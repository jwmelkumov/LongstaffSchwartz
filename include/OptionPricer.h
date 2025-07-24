#pragma once
class OptionPricer {
public:
    enum Type { CALL, PUT };
    OptionPricer(Type type, double strike);
    double payoff(double spot) const noexcept;
private:
    Type type_;
    double strike_;
};

