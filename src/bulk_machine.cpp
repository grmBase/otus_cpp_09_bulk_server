//---------------------------------------------------------------------------
#include <iostream>
#include <fstream> // вывод в файл
#include <filesystem>  // определяем наличие уже такого файла
//---------------------------------------------------------------------------
#include "bulk_machine.h"
//---------------------------------------------------------------------------
#include "sync_console.h" // выводим логи синхронизованно
//#include "async.h"
//---------------------------------------------------------------------------




// конструктор с одним параметром - там удобно так использовать
impl::t_bulk_machine::t_bulk_machine(size_t aun_stat_bulk_size)
  :m_un_stat_buld_size(aun_stat_bulk_size),
  m_un_dyn_depth(0)
{
  // зарезервируем. Типа чуть пооптимальнее с большой вероятностью
  m_vec_buffer.reserve(m_un_stat_buld_size);
}
//---------------------------------------------------------------------------


// деструктор, т.к. в конце иногда могут оставаться данные, и их нужно тоже распечатать
impl::t_bulk_machine::~t_bulk_machine()
{
  if(m_vec_buffer.size()) {
    clog::logout("<< eof detected. so purging final batch if any:");
    print_batch_and_clear();
  }
}
//---------------------------------------------------------------------------


void impl::t_bulk_machine::handle_instruction(const std::string& astr_inst)
{

  if (!astr_inst.length()) {
    clog::log_err("empty string in handle_instruction()");
    return;
  };


  if(astr_inst == "{") {

    // if we were in regular block:
    if (m_un_dyn_depth == 0) {
      clog::logout("comming dyn block, so purgint regular one if any:");
      print_batch_and_clear();
    }

    m_un_dyn_depth++;
    clog::logout("new dyn depth: " + std::to_string(m_un_dyn_depth));
    return;
  }

  if (astr_inst == "}") {
    if (m_un_dyn_depth >= 1) {
      m_un_dyn_depth--;
      clog::logout("} detected. new depth: " + std::to_string(m_un_dyn_depth));
    }
    else {
      // todo: hope test input will come in correct order. Otherwise ask how to handle such case
      clog::logout("some protocol violation detected. extra closing bracket. just ignored so far");
    }

    if (m_un_dyn_depth == 0) {
      clog::logout("dyn block is finished. printing content: ");
      print_batch_and_clear();
    }

    return;
  }

  
  // если инструкция обычная, то сохраняем её сначала в буфере:
  m_vec_buffer.push_back(std::move(astr_inst));

  if (m_un_dyn_depth == 0) {
    if (m_vec_buffer.size() >= m_un_stat_buld_size) {

      clog::logout("<< regular batch ready ----------");
      print_batch_and_clear();
    }
  }

  return;
}
//---------------------------------------------------------------------------


// выводим текущий буфер куда надо (в файл, в консоль...)
void impl::t_bulk_machine::print_batch_and_clear()
{

  if(!m_vec_buffer.size()) {
    clog::logout("nothing to purge, skipped");
    return;
  }

  std::string str_bulk;
  for(auto& curr : m_vec_buffer)
  {
    if (str_bulk.length()) {
      str_bulk += ", ";
    }
    str_bulk += curr;
  }


  // выведем в консоль:
  clog::log_info_always(str_bulk);


  // сохраняем в файл:
  int n_res = save_to_file(str_bulk);
  if (n_res) {
    clog::log_err("Error in save_to_file()");
  }


  //impl::push_to_console_conv(str_bulk);
  //impl::push_to_file_conv(str_bulk);

  m_vec_buffer.clear();
}
//---------------------------------------------------------------------------



// сохраняем в файл:
// выводим текущий буфер куда надо (в файл, в консоль...)
int impl::t_bulk_machine::save_to_file(const std::string& astr_info)
{

  //std::chrono::time_point begin = std::chrono::steady_clock::now().time_since_epoch();
  auto duration = std::chrono::steady_clock::now().time_since_epoch();
  //std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  std::string str_filename_base = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

  std::string str_filename = str_filename_base + ".txt";
  size_t un_postfix = 0;
  while (true) {
    std::filesystem::path p = std::filesystem::u8path(str_filename);
    std::error_code status;

    // if file not exist
    if (!std::filesystem::exists(p, status)) {
      break;
    }

    str_filename = str_filename_base + "_" + std::to_string(un_postfix) + ".txt";
    un_postfix++;
  }

  clog::logout(">> filename to save: " + str_filename);

  std::ofstream file;

  file.open(str_filename);
  file << astr_info;
  file.close();

  return 0;
}
//---------------------------------------------------------------------------


// узнать находимся ли мы в процессе обработки динамического блока
bool impl::t_bulk_machine::is_in_dyn_block_handling()
{
  return m_un_dyn_depth;
}
//---------------------------------------------------------------------------


// узнать есть ли данная строка начало динамического блока:
bool impl::t_bulk_machine::is_it_start_block_instr(const std::string& astr_instr)
{
  if (astr_instr == "{") {
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

