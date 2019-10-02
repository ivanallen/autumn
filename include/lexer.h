#include "token.h"

namespace autumn {

class Lexer {
public:
    Lexer(const std::string& input);
    Token next_token();
private:
    void read_char();
    bool is_letter(char c) const;
    bool is_digital(char c) const;
    std::string read_identifier();
    std::string read_number();
    void skip_whitespace();
private:
    std::string _input;

    char _ch = 0; // 当前读取的字符
    int _pos = 0; // 当前读取的字符位置
    int _read_pos = 0; // 即将要读取的字符位置
};

}; // namespace autumn
