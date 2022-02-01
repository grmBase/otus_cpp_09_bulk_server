//---------------------------------------------------------------------------
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
//---------------------------------------------------------------------------
#include "server.h"
#include "sync_console.h"
//---------------------------------------------------------------------------



int main(int argc, char* argv[])
{
  std::cout << "in main() start" << std::endl;

  if (argc != 3) {
    std::cerr << "Usage: server <port> <bulk_size>" << std::endl;
    return 1;
  }

  int n_port = std::atoi(argv[1]);
  int n_bulk_size = std::atoi(argv[2]);

  std::cout << "Detected params: tcp port: " << n_port << ", bulk_size: " << n_bulk_size << std::endl;


  try
  {
    // число потоков возьмём исходя из числа CPU
    const auto processor_count = std::thread::hardware_concurrency();
    std::cout << "num of CPU cores: " << processor_count << std::endl;

    t_server server(n_port, n_bulk_size, processor_count);

    int n_res = server.start_listen();
    if (n_res) {
      std::cout << "error in start_listen()" << std::endl;
      return n_res;
    }
    std::cout << "listen() completed OK" << std::endl;



    clog::logout("before server.run()");

    n_res = server.run();
    if (n_res) {
      clog::log_err("error in run()");
      return n_res;
    }
    clog::logout("server.run() passed OK");

    clog::logout("press any key to exit");

    do{
       std::cout << '\n' << "Press a key to continue...";
    } while (std::cin.get() != '\n');

    //int n_button = std::getchar();
    //clog::logout("pressed key: " + std::to_string(n_button));

    clog::logout("going to stop. Before server.stop()");
    server.stop();

    clog::logout("after stop");
  }
  catch(const std::exception& aexc)
  {
    clog::log_err("caught exeption: " + std::string(aexc.what()));
    return -33;
  }

  return 0;
}
//---------------------------------------------------------------------------
