//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "tcp_connect.h"   // наш хидер
//---------------------------------------------------------------------------
#include "sync_console.h" // выводим в консоль синхронно
#include "server.h" // выводим в консоль синхронно
//---------------------------------------------------------------------------


tcp_connect::tcp_connect(boost::asio::ip::tcp::socket a_socket, t_server& a_server)
  : m_socket(std::move(a_socket)), 
    m_server(a_server)
{
}
//---------------------------------------------------------------------------


void tcp_connect::start()
{
  do_read();
}
//---------------------------------------------------------------------------

void tcp_connect::tcp_connect::do_read()
{
  auto self(shared_from_this());
  m_socket.async_read_some(boost::asio::buffer(m_buf, c_un_buf_length),
    [this, self](boost::system::error_code a_error, std::size_t a_readed)
    {
      if(a_error) {
        t_sync_console::log_err("error in asyn_read_some(). code: " + std::to_string(a_error.value()) + ", transl: " + a_error.message());
        return; 
      }
     
      t_sync_console::logout("num received: " + std::to_string(a_readed) + ", content: " + std::string{ (char*)m_buf, a_readed });

      // обрабатываем:
      int n_err = handle_some((const char*)m_buf, a_readed);

      // Мы ничего не отвечаем в ответ, поэтому просто продолжаем запрашивать чего-нибудь на чтение:
      do_read();
      //do_write(a_readed);
      
    });
}
//---------------------------------------------------------------------------

/*
void tcp_connect::do_write(std::size_t a_len)
{
  auto self(shared_from_this());
  boost::asio::async_write(m_socket, boost::asio::buffer(m_buf, a_len),
    [this, self](boost::system::error_code a_error, std::size_t a_written)
    {
      if(a_error) {
        t_sync_console::log_err("error in async_write(). code: " + std::to_string(a_error.value()) + ", transl: " + a_error.message());
        return;
      }

      t_sync_console::logout("num written: " + std::to_string(a_written) + ", content: " + std::string{ (char*)m_buf, a_written });

      do_read();
    });
}
//---------------------------------------------------------------------------
*/


// Обрабатываем то, что к нам пришло:
int tcp_connect::handle_some(const char* ap_data, std::size_t a_un_size)
{
  // создаём полную строчку из предыдущего буфера плюс то, что нам передали:
  std::string str_curr = m_buf_not_ready_yet + std::string(ap_data, a_un_size);

  // ищем переводы строк, и обрабатываем:
  while (true) {

    auto iter1 = str_curr.find("\n");
    auto iter2 = str_curr.find("\r\n");

    // Если ничего не нашли:
    if (iter1 == std::string::npos && iter2 == std::string::npos) {
      break;
    }


    if (iter2 != std::string::npos) {
      std::string str_new_to_handle = str_curr.substr(0, iter2);

      m_server.handle_instruction(str_new_to_handle);

      str_curr = str_curr.substr(iter2 + std::strlen("\r\n"), std::string::npos);

      continue;
    }


    if (iter1 != std::string::npos) {
      std::string str_new_to_handle = str_curr.substr(0, iter1);

      m_server.handle_instruction(str_new_to_handle);

      str_curr = str_curr.substr(iter1 + std::strlen("\r"), std::string::npos);

      continue;
    }

  }

  m_buf_not_ready_yet = str_curr; // запоминаем остаток

  return 0;
};
//---------------------------------------------------------------------------