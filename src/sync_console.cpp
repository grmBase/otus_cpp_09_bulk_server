//---------------------------------------------------------------------------
#include <iostream>
//---------------------------------------------------------------------------
#include "sync_console.h"
//---------------------------------------------------------------------------



// пока включим отладку:
#define DBG_LOGGING

void t_sync_console::log_info_inst(const std::string_view& astr_info)
{

#ifdef DBG_LOGGING
  std::lock_guard<std::mutex> lock(m_mut_cout);

  std::cout << astr_info << " [tid:" << std::this_thread::get_id() << "]" << std::endl;
#endif
}
//---------------------------------------------------------------------------


void t_sync_console::logout_inst(const std::string_view& astr_info)
{
  log_info(astr_info);
}
//---------------------------------------------------------------------------


// 
void t_sync_console::log_err_inst(const std::string_view& astr_info)
{
#ifdef DBG_LOGGING
  std::lock_guard<std::mutex> lock(m_mut_cerr);

  std::cerr << astr_info << " [tid:" << std::this_thread::get_id() << "]" << std::endl;
#endif
}
//---------------------------------------------------------------------------



void t_sync_console::logout(const std::string_view& astr_info)
{
#ifdef DBG_LOGGING
  gp_log->logout_inst(astr_info);
#endif
}
//---------------------------------------------------------------------------

void t_sync_console::log_info(const std::string_view& astr_info)
{
#ifdef DBG_LOGGING
  gp_log->log_info_inst(astr_info);
#endif
}
//---------------------------------------------------------------------------

void t_sync_console::log_err(const std::string_view& astr_info)
{
#ifdef DBG_LOGGING
  gp_log->log_err_inst(astr_info);
#endif
}
//---------------------------------------------------------------------------
