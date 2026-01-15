#include <variant>
#include <cstddef>
#include <string>
#include <sstream>
#include <stdexcept>

enum class infinite {
    alephnull
};

class Cardinal {
private:
    using finite = size_t;
    std::variant<finite, infinite> value;

public:
    Cardinal() : value(finite{0}) {}
    Cardinal(finite v) : value(v) {}
    Cardinal(infinite v) : value(v) {}

    bool is_infinite() const {
        return std::holds_alternative<infinite>(value);
    }

    finite get_value() const {
        if (is_infinite())
            throw std::logic_error("Cardinal is infinite");
        return std::get<finite>(value);
    }

    std::string to_string() const {
        std::ostringstream oss;
        if (is_infinite())
            oss << "ℵ₀";
        else
            oss << std::get<finite>(value);
        return oss.str();
    }

    bool operator==(const Cardinal& other) const {
        if (is_infinite() && other.is_infinite()) return true;
        if (is_infinite() != other.is_infinite()) return false;
        return std::get<finite>(value) == std::get<finite>(other.value);
    }

    bool operator<(const Cardinal& other) const {
        if (is_infinite() && other.is_infinite()) return false;
        if (is_infinite()) return false;
        if (other.is_infinite()) return true;
        return std::get<finite>(value) < std::get<finite>(other.value);
    }

    Cardinal operator+(const Cardinal& other) const {
        if (is_infinite() || other.is_infinite())
            return Cardinal(infinite::alephnull);
        return Cardinal(
            std::get<finite>(value) + std::get<finite>(other.value)
        );
    }
};
