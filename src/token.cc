#include "token.h"

namespace autumn {

bool Token::operator==(const Token& rhs) const {
    return type == rhs.type && literal == rhs.literal;
}

}
