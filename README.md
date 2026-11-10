# TurboFill Fuel Station Management System

A menu-driven console program in C that simulates a day of operations at a fuel station. Written as a course project at Florida International University.

## Features

- **Daily operations:** serve customers by fuel type (Regular, Premium, Diesel), quantity, and vehicle type
- **Pricing:** per-gallon rates, state fuel tax by vehicle type, bulk discounts (30+ and 50+ gallons), and a loyalty discount
- **Rules and validation:** per-transaction gallon caps, stock checks, a block on Motorcycle + Diesel, a confirmation for Car + Diesel, and re-prompting on invalid input
- **Receipts:** itemized base cost, tax, and discounts for each transaction
- **Station management:** fuel inventory status with LOW and CRITICAL stock alerts
- **Day summary:** customers served, gallons sold, revenue, tax, discounts, net income, and vehicle breakdown

## Build and run

Requires `gcc` and `make`.

```bash
make        # compiles with -std=c99 -Wall -Wextra -Werror -pedantic
make run    # builds and runs ./fuel_station
make clean  # removes the binary
```
