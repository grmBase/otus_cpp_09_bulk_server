//---------------------------------------------------------------------------
#include <memory> // make_shared 
#include <string> // to_string()
#include <boost/asio.hpp>
//---------------------------------------------------------------------------
#include "server.h" // наш хедер
//---------------------------------------------------------------------------
#include "tcp_connect.h"
#include "sync_console.h" // выводим логи синхронизованно
//---------------------------------------------------------------------------



t_server::t_server(uint16_t a_port, size_t a_batch_size, size_t a_num_of_threads)
  : m_acceptor(m_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), a_port)),
    m_threads{a_num_of_threads},
    m_machine(a_batch_size)
{
}
//---------------------------------------------------------------------------


t_server::~t_server()
{
  t_sync_console::logout("before join() threads");

  for (auto& curr : m_threads) {
    curr.join();
  };

  t_sync_console::logout("after join() threads");
}
//---------------------------------------------------------------------------


int t_server::start_listen()
{
  do_accept();

  return 0;
};
//---------------------------------------------------------------------------

void t_server::do_accept()
{
  m_acceptor.async_accept(
    [this](boost::system::error_code a_error_code, boost::asio::ip::tcp::socket a_socket)
    {
      if(a_error_code) {
        t_sync_console::log_err("Error after accept()");
        return;
      }
      
      
      t_sync_console::logout("<< income connection. remote endpoint: " + 
        a_socket.remote_endpoint().address().to_string() + ":" +
        std::to_string(a_socket.remote_endpoint().port()));
    

      std::make_shared<tcp_connect>(std::move(a_socket), *this)->start();

      do_accept();
    });
}
//---------------------------------------------------------------------------


int t_server::run()
{
  t_sync_console::logout("before start working threads");

  // запускаем нити:
  for (auto& curr : m_threads) {
    curr = std::thread([this] {this->asio_thread_work(); });
  };

  t_sync_console::logout("after all working threads has been started");

  return 0;
};
//---------------------------------------------------------------------------


int t_server::stop()
{
  t_sync_console::logout("before stop() io_context");
  m_io_context.stop();
  t_sync_console::logout("after stop() io_context");

  return 0;
};
//---------------------------------------------------------------------------


void t_server::asio_thread_work()
{

  t_sync_console::logout("in start of thread work()");

  try
  {
    boost::system::error_code ec;

    t_sync_console::logout("before io_context.run()");
    m_io_context.run(ec);
    
    t_sync_console::logout("after io_context.run(), res code: " + std::to_string(static_cast<int>(ec.value())) + ", transl: " + ec.message());
  }
  catch(const std::exception& aexc)
  {
    t_sync_console::log_err(std::string("exception in work exception info: ") + aexc.what());
  }
}
//---------------------------------------------------------------------------


void t_server::handle_instruction(const std::string& astr_instr)
{
  std::lock_guard lock(m_mut_cout);
  m_machine.handle_instruction(astr_instr);
}
//---------------------------------------------------------------------------