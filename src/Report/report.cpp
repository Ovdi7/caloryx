#include "report.h"

Report::Report() : totalCalories(0.0), proteins(0.0), fats(0.0), carbs(0.0) {
}

Report::Report(double totalCalories, double proteins, double fats, double carbs)
    : totalCalories(totalCalories), proteins(proteins), fats(fats), carbs(carbs) {
}

double Report::getTotalCalories() const {
    return totalCalories;
}

double Report::getProteins() const {
    return proteins;
}

double Report::getFats() const {
    return fats;
}

double Report::getCarbs() const {
    return carbs;
}
