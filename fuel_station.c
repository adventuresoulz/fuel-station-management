#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


    //TurboFill Fuel Station Management System


/********************* FIXED PRICING & POLICIES ************************/

// fuel rates 
const double REG_RATE  = 3.00;
const double PREM_RATE = 4.00;
const double DSL_RATE  = 3.50;

// state fuel tax rates 
const int TAX_MOTORCYCLE = 2;
const int TAX_CAR        = 3;
const int TAX_TRUCK      = 1;

// bulk discount 
const int DISC_30_PLUS = 5;
const int DISC_50_PLUS = 10;

// loyalty discount 
const int LOYALTY_DISC = 2;

// caps per transaction
const int MAX_GAL_GAS    = 200; // regular & premium
const int MAX_GAL_DIESEL = 400;

/********************* INVENTORY STATE  ************************/

double regInitial   = 1000.0;
double premInitial  = 1000.0;
double dslInitial   = 1000.0;

double regSold      = 0.0;
double premSold     = 0.0;
double dslSold      = 0.0;

/********************* DAILY TOTALS / METRICS **************************/

int    totalCustomers   = 0;
double totalGallons     = 0.0;
double salesRevenue     = 0.0; // base
double taxCollected     = 0.0; // remitted to government ...(lol)
double totalDiscounts   = 0.0; // bulk + loyalty
double customerPayments = 0.0; // what customers actually pay 

int motorcyclesCount = 0;
int carsCount        = 0;
int trucksCount      = 0;

/***************************** UTILITIES *******************************/

//----- LONG MENU BORDER APPROX 70 -----
void longBorder(void) {
    for (int i = 0; i < 43; ++i) putchar('=');
}

//----- flush input line -----
void flushLine(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// read integer in  with prompt
int readIntInRange(const char *prompt, int lo, int hi){
    int x;
    for(;;){
        printf("%s", prompt);
        if (scanf("%d", &x) == 1 && x >= lo && x <= hi) { flushLine(); return x; }
        printf("Invalid input. Please enter a number between %d and %d.\n", lo, hi);
        flushLine();
    }
}

// read double in [lo, hi] with prompt
double readDoubleInRange(const char *prompt, double lo, double hi){
    double v;
    for(;;){
        printf("%s", prompt);
        if (scanf("%lf", &v) == 1 && v >= lo && v <= hi) { flushLine(); return v; }
        printf("Invalid input. Please enter a value between %.2f and %.2f.\n", lo, hi);
        flushLine();
    }
}

// yes/no as 1 or 0 
int readBinary01(const char *prompt){
    int b;
    for(;;){
        printf("%s", prompt);
        if (scanf("%d", &b) == 1 && (b == 0 || b == 1)) { flushLine(); return b; }
        printf("Invalid input. Enter 1 for Yes or 0 for No.\n");
        flushLine();
    }
}

/**************************** QUICK PRINTS *****************************/

void printFuelRates(void) {
    printf("Fuel Rates:\n");
    printf("  Regular Gas : $%.2f/gal (Max: %d gal/transaction)\n", REG_RATE,  MAX_GAL_GAS);
    printf("  Premium Gas : $%.2f/gal (Max: %d gal/transaction)\n", PREM_RATE, MAX_GAL_GAS);
    printf("  Diesel      : $%.2f/gal (Max: %d gal/transaction)\n", DSL_RATE,  MAX_GAL_DIESEL);
}

void printTaxRates(void) {
    printf("State Fuel Tax Rates:\n");
    printf("  Motorcycle  : %d%%\n", TAX_MOTORCYCLE);
    printf("  Car         : %d%%\n", TAX_CAR);
    printf("  Truck       : %d%%\n", TAX_TRUCK);
}

void printBulkDiscounts(void) {
    printf("Bulk Discount Rates:\n");
    printf("  30+ gallons : %d%% off total\n", DISC_30_PLUS);
    printf("  50+ gallons : %d%% off total\n", DISC_50_PLUS);
}

/*********************** STARTUP / WELCOME DISPLAY *********************/

void welcomeDisplay(void){
    for (int i =0; i < 58; i++){
	printf("=");
}
printf("\n");
    printf("    WELCOME TO TurboFill FUEL STATION MANAGEMENT SYSTEM\n");
    
  for (int i =0; i < 58; i++){
	printf("=");
}
printf("\n");
    printf("\n\n");
    printf("========= CURRENT PRICING & POLICIES =========\n");
    printFuelRates(); printf("\n");
    printTaxRates();  printf("\n");
    printBulkDiscounts(); printf("\n");
    printf("Loyalty Program: Additional %d%% off total\n", LOYALTY_DISC);
    longBorder(); printf("\n");
}

/*********************** STOCK / STATUS HELPERS ************************/

double remainingStock(int fuel){ // 1=Reg, 2=Prem, 3=Diesel
    if (fuel == 1) return regInitial  - regSold;
    if (fuel == 2) return premInitial - premSold;
    return dslInitial - dslSold;
}

double fuelRate(int fuel){ // 1=Reg, 2=Prem, 3=Diesel
    if (fuel == 1) return REG_RATE;
    if (fuel == 2) return PREM_RATE;
    return DSL_RATE;
}

const char* fuelName(int fuel){
    if (fuel == 1) return "Regular Gas";
    if (fuel == 2) return "Premium Gas";
    return "Diesel";
}

const char* vehicleName(int v){ // 1=Motorcycle, 2=Car, 3=Truck
    if (v == 1) return "Motorcycle";
    if (v == 2) return "Car";
    return "Truck";
}

int vehicleTaxPct(int v){
    if (v == 1) return TAX_MOTORCYCLE;
    if (v == 2) return TAX_CAR;
    return TAX_TRUCK;
}

int bulkDiscPct(double gals){
    if (gals >= 50.0) return DISC_50_PLUS;
    if (gals >= 30.0) return DISC_30_PLUS;
    return 0;
}

const char* stockStatus(double cur){
    if (cur < 50.0)  return "CRITICAL";
    if (cur < 200.0) return "LOW";
    return "NORMAL";
}

/*********************** INVENTORY DISPLAY (ADMIN) *********************/

void printFuelInventoryStatus(void){
    longBorder(); printf("\n");
    printf("              FUEL INVENTORY STATUS\n");
    longBorder(); printf("\n");
    printf(" Stock Status Thresholds:\n");
    printf("   CRITICAL: < 50 gallons\n");
    printf("   LOW: < 200 gallons\n");
    printf("   NORMAL: >= 200 gallons\n");
    longBorder(); printf("\n");

    // REGULAR
    printf(" REGULAR GAS:\n");
    printf("   Current Stock    : %.1f   gal\n", regInitial - regSold);
    printf("   Sold Today       : %.1f      gal\n", regSold);
    printf("   Stock Status     : %s\n",    stockStatus(regInitial - regSold));
    longBorder(); printf("\n");

    // PREMIUM
    printf(" PREMIUM GAS:\n");
    printf("   Current Stock    : %.1f   gal\n", premInitial - premSold);
    printf("   Sold Today       : %.1f      gal\n", premSold);
    printf("   Stock Status     : %s\n",    stockStatus(premInitial - premSold));
    longBorder(); printf("\n");

    // DIESEL
    printf(" DIESEL:\n");
    printf("   Current Stock    : %.1f   gal\n", dslInitial - dslSold);
    printf("   Sold Today       : %.1f      gal\n", dslSold);
    printf("   Stock Status     : %s\n",    stockStatus(dslInitial - dslSold));
    longBorder(); printf("\n");

    // Alerts
    if (regInitial - regSold < 50.0 || premInitial - premSold < 50.0 || dslInitial - dslSold < 50.0) {
        printf("*** CRITICAL ALERT: Emergency fuel stock level! ***\n");
        if (regInitial - regSold  < 50.0)  printf("   Regular Gas: %.1f gal remaining\n", regInitial - regSold);
        if (premInitial - premSold< 50.0)  printf("   Premium Gas: %.1f gal remaining\n", premInitial - premSold);
        if (dslInitial - dslSold  < 50.0)  printf("   Diesel     : %.1f gal remaining\n", dslInitial - dslSold);
    } else if (regInitial - regSold < 200.0 || premInitial - premSold < 200.0 || dslInitial - dslSold < 200.0) {
        printf("*** LOW STOCK WARNING ***\n");
        if (regInitial - regSold  < 200.0) printf("   Regular Gas: %.1f gal remaining\n", regInitial - regSold);
        if (premInitial - premSold< 200.0) printf("   Premium Gas: %.1f gal remaining\n", premInitial - premSold);
        if (dslInitial - dslSold  < 200.0) printf("   Diesel     : %.1f gal remaining\n", dslInitial - dslSold);
    }
}

/************************* DAY SUMMARY *************************/

void printDaySummary(void){
    longBorder(); printf("\n");
    printf("              CURRENT DAY SUMMARY\n");
    longBorder(); printf("\n");

    printf(" Total Customers    : %d\n", totalCustomers);
    printf(" Total Fuel Sold    : %.1f   gal\n", totalGallons);
    printf(" Sales Revenue      : $%.2f\n", salesRevenue);
    printf(" State Tax Collected: $%.2f\n", taxCollected);
    printf(" Total Discounts    : $%.2f\n", totalDiscounts);
    longBorder(); printf("\n");

    printf(" Customer Payments  : $%.2f\n", customerPayments);
    printf(" Station Net Income : $%.2f\n", customerPayments - taxCollected);
    longBorder(); printf("\n");

    printf(" Vehicle Distribution:\n");
    printf(" Motorcycles        : %d\n", motorcyclesCount);
    printf(" Cars               : %d\n", carsCount);
    printf(" Trucks             : %d\n", trucksCount);
    longBorder(); printf("\n");

    if (totalCustomers > 0) {
        printf(" Average Payment    : $%.2f\n", customerPayments / totalCustomers);
        printf(" Avg Fuel/Customer  : %.2f   gal\n", totalGallons / totalCustomers);
    } else {
        printf(" Average Payment    : $0.00\n");
        printf(" Avg Fuel/Customer  : 0.00   gal\n");
    }
    longBorder(); printf("\n");
}

/********************* DAILY OPS: SERVE NEW CUSTOMER *******************/

//************ DAILY OPERATIONS: SERVE NEW CUSTOMER ************

void serveNewCustomer(void){
    int fuelChoice = 0;     // 1=Reg 2=Prem 3=Diesel
    int vehicle    = 0;     // 1=Moto 2=Car  3=Truck
    double gallons = 0.0;
    int isLoyal    = 0;     // 1/0 per spec

    totalCustomers++;

    longBorder(); printf("\n");
    printf("              --- CUSTOMER #%03d ---\n", totalCustomers);
    longBorder(); printf("\n");

    //----- FUEL SELECTION -----
    double regRem  = remainingStock(1);
    double premRem = remainingStock(2);
    double dslRem  = remainingStock(3);

    printf("Select Fuel Type:\n");
    if (regRem <= 0.0)  printf("1. Regular Gas ($%.2f/gal) - OUT OF STOCK\n", REG_RATE);
    else               printf("1. Regular Gas ($%.2f/gal) - Stock: %.1f gal\n", REG_RATE, regRem);

    if (premRem <= 0.0) printf("2. Premium Gas ($%.2f/gal) - OUT OF STOCK\n", PREM_RATE);
    else               printf("2. Premium Gas ($%.2f/gal) - Stock: %.1f gal\n", PREM_RATE, premRem);

    if (dslRem <= 0.0)  printf("3. Diesel ($%.2f/gal)      - OUT OF STOCK\n", DSL_RATE);
    else               printf("3. Diesel ($%.2f/gal)      - Stock: %.1f gal\n", DSL_RATE, dslRem);

    fuelChoice = readIntInRange("Fuel Choice: ", 1, 3);

    // prevent selecting out of stock
    while (remainingStock(fuelChoice) <= 0.0){
        printf("Selected fuel is OUT OF STOCK. Choose a different fuel.\n");
        fuelChoice = readIntInRange("Fuel Choice: ", 1, 3);
    }

    //----- QUANTITY INPUT  -----
    int maxCap = (fuelChoice == 3) ? MAX_GAL_DIESEL : MAX_GAL_GAS;
    char qtyPrompt[64];
    snprintf(qtyPrompt, sizeof(qtyPrompt), "Enter quantity in gallons (Max: %d): ", maxCap);
    gallons = readDoubleInRange(qtyPrompt, 0.01, (double)maxCap);

    // must be <= remaining stock
    while (gallons > remainingStock(fuelChoice)) {
        printf("Invalid quantity! Not enough stock. Available: %.1f gal\n", remainingStock(fuelChoice));
        gallons = readDoubleInRange(qtyPrompt, 0.01, (double)maxCap);
    }

    //----- VEHICLE TYPE + COMPATIBILITY -----
    printf("Select Vehicle Type:\n");
    printf("1. Motorcycle (%d%% state fuel tax)\n", TAX_MOTORCYCLE);
    printf("2. Car (%d%% state fuel tax)\n",        TAX_CAR);
    printf("3. Truck (%d%% state fuel tax)\n",      TAX_TRUCK);
    vehicle = readIntInRange("Vehicle Choice: ", 1, 3);

    // Motorcycle + Diesel = prohibited
    if (vehicle == 1 && fuelChoice == 3){
        printf("Error: Motorcycles cannot refuel with Diesel. Please choose a different fuel or vehicle.\n");
        return;
    }
    // Car + Diesel = confirmation
    if (vehicle == 2 && fuelChoice == 3){
        int ok = readBinary01("Confirm Car + Diesel? (1=Yes, 0=No): ");
        if (!ok) {
            printf("Transaction cancelled.\n");
            return;
        }
    }

    //----- LOYALTY -----
    isLoyal = readBinary01("Is customer a loyalty member? (1=Yes, 0=No): ");

    //----- CALCULATIONS -----
    double rate = fuelRate(fuelChoice);
    double base = rate * gallons;
    double tax  = base * (vehicleTaxPct(vehicle) / 100.0);

    int discBulkPct = bulkDiscPct(gallons);
    int discLoyalPct = isLoyal ? LOYALTY_DISC : 0;
    int combinedDiscPct = discBulkPct + discLoyalPct;

    double preDiscount = base + tax;
    double discountAmt = preDiscount * (combinedDiscPct / 100.0);
    double totalPay    = preDiscount - discountAmt;

    //----- INVENTORY & TOTALS UPDATE -----
    if (fuelChoice == 1) regSold  += gallons;
    else if (fuelChoice == 2) premSold += gallons;
    else dslSold += gallons;

    totalGallons     += gallons;
    salesRevenue     += base;
    taxCollected     += tax;
    totalDiscounts   += discountAmt;
    customerPayments += totalPay;

    if (vehicle == 1) motorcyclesCount++;
    else if (vehicle == 2) carsCount++;
    else trucksCount++;

    //----- RECEIPT (format per spec style) -----
    longBorder(); printf("\n");
    printf("               FUEL RECEIPT #%03d\n", totalCustomers);
    longBorder(); printf("\n");

    printf(" Fuel Type    : %s\n",     fuelName(fuelChoice));
    printf(" Vehicle Type : %s\n",     vehicleName(vehicle));
    printf(" Quantity     : %.2f   gal\n", gallons);
    printf(" Rate         : $%.2f  /gal\n", rate);
    printf(" Loyalty      : %s\n",     isLoyal ? "Yes" : "No");
    longBorder(); printf("\n");

    printf(" Base Cost    : $%.2f\n", base);
    printf(" State Tax    : $%.2f\n", tax);
    if (discBulkPct > 0)  printf(" Bulk Discount: -$%.2f\n", preDiscount * (discBulkPct / 100.0));
    else                  printf(" Bulk Discount: -$%.2f\n", 0.0);
    if (discLoyalPct > 0) printf(" Loyalty Disc.: -$%.2f\n", preDiscount * (discLoyalPct / 100.0));
    else                  printf(" Loyalty Disc.: -$%.2f\n", 0.0);
    longBorder(); printf("\n");

    printf(" TOTAL PAYABLE: $%.2f\n", totalPay);
    longBorder(); printf("\n");

    printf("Transaction completed successfully!\n");
}

/***************************** DAILY OPS MENU **************************/

void dailyOpsMenu(void){
    for(;;){
        printf("========= DAILY OPERATIONS MODE ===========\n");
        printf("1. Serve New Customer\n");
        printf("2. View Current Day Summary\n");
        printf("3. Return to Main Menu\n");
        longBorder(); printf("\n");
        int choice = readIntInRange("Choice: ", 1, 3);

        if (choice == 1) {
            serveNewCustomer();
        } else if (choice == 2) {
            printDaySummary();
        } else {
            return; // back to main menu
        }
        printf("\n");
    }
}

/***************************** MAIN MENU *******************************/

void mainMenu(void){
    for(;;){
        printf("\n");
        printf("========= SYSTEM MODE SELECTION =========\n");
        printf("1. Daily Operations (Customer Service)\n");
        printf("2. Station Management (Administrative)\n");
        printf("3. Exit System\n");
        longBorder(); printf("\n\n");

        int choice = readIntInRange("Select Mode: ", 1, 3);

        if (choice == 1) {
            dailyOpsMenu();
        } else if (choice == 2) {
            printFuelInventoryStatus();
        } else {
            printf("Thank you for using the TurboFill FUEL STATION MANAGEMENT SYSTEM\nGoodbye!\n");
            exit(0);
        }
    }
}

/********************************* MAIN ********************************/

int main(void){
    welcomeDisplay();
    mainMenu();
    return 0;
}
