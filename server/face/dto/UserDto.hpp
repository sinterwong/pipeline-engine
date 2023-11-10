/**
 * @file UserDto.hpp
 * @author Sinter Wong (sintercver@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-11-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __SERVER_FACE_DTO_USER_DTO_HPP_
#define __SERVER_FACE_DTO_USER_DTO_HPP_

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include <string>

namespace server::face {
#include OATPP_CODEGEN_BEGIN(DTO) ///< Begin DTO codegen section

class UserDto : public oatpp::DTO {
  DTO_INIT(UserDto, DTO)

  DTO_FIELD(Int32, id);
  DTO_FIELD(String, idNumber);
  DTO_FIELD(String, feature);

public:
  std::string toString() const {
    return "UserDto(\n"
           "  id: " +
           std::to_string(id) + ",\n" + "  idNumber: " + idNumber->c_str() +
           "\n)";
  }
};
#
} // namespace server::face

#endif