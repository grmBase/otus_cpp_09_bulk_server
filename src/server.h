//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include <boost/asio.hpp>
//---------------------------------------------------------------------------
#include "engine/bulk_machine.h"
//---------------------------------------------------------------------------


// todo: потом взять из системы число cpu
//const int c_n_num_of_work_threads = 4;

class t_server
{
  public:

    t_server(uint16_t a_port, size_t a_batch_size, size_t a_num_of_threads);

    ~t_server();

    // запускаем слушание порта:
    int start_listen();

    // то что вызывют нитки когда крутятся
    void asio_thread_work();

    int run();

    int stop();


    void handle_instruction(const std::string& astr_instr);

  private:

    void do_accept();



    // *********** данные ***************
    uint16_t m_port;

    // основной объект asio
    boost::asio::io_context m_io_context;

    // слушатель входящих запросов tcp
    boost::asio::ip::tcp::acceptor m_acceptor;

    // здесь храним список рабочих нитей
    std::vector<std::thread> m_threads;



    // мьютекс доступк к одной машине
    mutable std::mutex m_mut_cout;

    // машина построчной отработки логики пачек
    impl::t_bulk_machine m_machine;

};
//---------------------------------------------------------------------------