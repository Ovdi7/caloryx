#pragma once
#include <vector>
#include <string>
#include "Product.h"

class ProductDatabase {
private:
    std::vector<Product> products;

public:
    ProductDatabase() {
        // Початкова база продуктів
        products.push_back({"Куряче філе", 165.0, 31.0, 3.6, 0.0});
        products.push_back({"Гречка (варена)", 132.0, 4.5, 1.1, 28.0});
        products.push_back({"Яблуко", 52.0, 0.3, 0.2, 14.0});
        products.push_back({"Яйце куряче", 155.0, 13.0, 11.0, 1.1});
    }

    void addProduct(const Product& product) {
        products.push_back(product);
    }

    const std::vector<Product>& getAllProducts() const {
        return products;
    }
};