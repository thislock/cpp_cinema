
#ifndef ERROR_H
#define ERROR_H

#include <string>

void showErrorPopup(const std::string& message);
int showSdlError(const std::string & error_type);

#endif