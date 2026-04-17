#pragma once

class Report {
private:
    double totalCalories;
    double proteins;
    double fats;
    double carbs;

public:
    Report();
    Report(double totalCalories, double proteins, double fats, double carbs);

    double getTotalCalories() const;
    double getProteins() const;
    double getFats() const;
    double getCarbs() const;
};
