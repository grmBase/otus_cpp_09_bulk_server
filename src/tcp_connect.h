//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include <boost/asio.hpp>
//---------------------------------------------------------------------------
class t_server;
//---------------------------------------------------------------------------
const size_t c_un_buf_length = 1024;
//---------------------------------------------------------------------------



class tcp_connect : public std::enable_shared_from_this<tcp_connect>
{

public:

  tcp_connect(boost::asio::ip::tcp::socket a_socket, t_server& a_server);

  void start();

private:

  // заказываем чего-то читать:
  void do_read();
  // что-то пишем - пока не надо в этом задании:
  //void do_write(std::size_t length);


  // Обрабатываем то, что к нам пришло:
  int handle_some(const char* ap_data, std::size_t a_un_size);



  // ********************** Данные *******************************

  boost::asio::ip::tcp::socket m_socket;

  // буфер, куда читаем из сокета:
  uint8_t m_buf[c_un_buf_length];

  // строчка, которая ещё не готова:
  std::string m_buf_not_ready_yet;

  // ссылка на основной сервер, чтобы всякие вычитанные строки отправлять на обработку
  t_server& m_server;
};
//---------------------------------------------------------------------------

