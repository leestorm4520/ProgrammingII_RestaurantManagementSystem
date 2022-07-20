/*
    Ryan Eggett, Savannah Salinas, John Le
    Course: COSC 1436.001 Programming Fundamentals 1
    Final Project: Restaurant Management Software (RMS)
    Due Date: August 11, 2019
    Instructor: Korrine Caruso

    Purpose: This program assists with the management of a pizza restaurant with various functions including order entry,
             point of sale transactions, sales reports and more.
    Input:   Users will input integers as chars for menus as well as integers and floats for values of entities such as ingredients and payments.
    Output:  Output will be displayed on screen for continual feedback as well as certain data to be stored in files on computer.
*/

/*
DEV NOTES:

TO DO FOR PROJECT:
1.	Flowcharts
2.	Pseudocode
3.	Code
4.	PowerPoint Presentation
5.	Project Report
6.	Team Evaluation Form

VERSION NOTES:
8/3/19 RE - Organized and commented the whole program. Moved some of the functions around to match what the intended flow of the program might be. Also,
            implemented a constant for the CLS command. I put the two that I know of there. You can comment out either one to get it working or you may have to look
            up another keyword if your compiler won't recognize either of those.

8/1/19 RE - Worked more on the end part of placeOrder and viewCart as well as starting payForOrder. The placeOrder function now returns an instance of Order
            and loads it directly into the queue, which is a vector called orderQueue. The payForOrder function reads the orders from this vector of Orders.

7/31/19 RE - Added pQuantity (for pizza quantity) to makePizza function, which allows user to order multiple of the same type of pizza.
             Added price constants as floats for large, medium, and personal pan pizzas, toppings, and tax. They are at the top of the program in ALL_CAPS.
             Created 3 new class functions for Pizza called getPizzaPrice, dispPizzaPrice, and dispPizzaPriceDetails. getPizzaPrice looks at the private
             variables inside of the Pizza class, adds up the pizza and toppings prices and saves them as a private float inside the Pizza class.
             dispPizzaPrice just returns a float (the pPrice). And dispPizzaPriceDetails doesn't return anythings, but displays a small readout/ breakdown
             of the cost of the pizza to the screen.
             So now when the list of Pizza instances are passed to the cart/queue, there will be a private variable you can access (pPrice) to know the price of the
             pizza, so you can add it to the prices of the other pizzas/items in the cart. To access this variable you'll just call curPizza.dispPizzaPrice();

7/30/19 RE - Finished makePizza(). You can now fully make a pizza, add/remove toppings, modify size and dough type, add multiple pizzas, and view them
             inside the cart.

7/29/19 RE - Added comments to the beginning of the makePizza function to help clarify Class functionality. Continued working on functions inside of
             the Pizza class, that makePizza will call to customize the pizza.
             Worked on setPizzaToppings custom function for the Pizza class. It is almost finished. It makes use of vectors, but we can easily change
             them to arrays if we decide that that is better. So now toppings can be added to the pizza.
             More version notes in the shared folder: "version notes.txt"
*/

#include <iostream>
#include <stdlib.h>           // Needed for system("CLS");
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
//#include <cstdlib> // maybe?
using namespace std;

const float PRICE_L_PIZZA = 12.00, PRICE_M_PIZZA = 9.00, PRICE_PP_PIZZA = 5.50, PRICE_TOPPING = 0.75, TAX = 1.0825;
const char CLEAR[] = {'C', 'L', 'S', '\0'}; // Used as a constant for the "system("CLS");" command. Other compilers use a different keyword for the clear command. To change the keyword, comment out this line and uncomment the next line.
//const char CLEAR[] = {'c', 'l', 'e', 'a', 'r', '\0'};     // The MinGW/GNU GCC compiler is the compiler we use for class. Our keyword is "CLS". The Clang compiler and maybe others use "clear". These are char arrays and are case-sensitive.

// Class Prototypes
class Order;
class Pizza;

// The order of the functions here is in the intended flow of the program. It is also the same order you'll find the function definitions, below (after the class definitions).
// Function protoypes           // Program map:
void mainMenu(bool &refQuit); // Called by main. Gets passed a reference to the bool variable "quit" (used for sending to endProg, which changes it to true if user chooses to quit program.
Order placeOrder();            // Called by mainMenu. Returns a complete instance of Order "curOrder", ready to be put into the queue for payForOrder(). Function allows for creation of food items that get moved to a "cart" which is assigned to a customer.
void foodMenu(Order& refCurrentOrder); // Called by placeOrder. Gets passed the instance of Order "curOrder" Function. Displays the 4 food options and runs the appropriate function to make them. Ie. makePizza()
Pizza makePizza();             // Called by placeOrder. Returns a complete Pizza instance. Function lets user customize a pizza and add it to the cart.
void dispAllPizzas(vector <Pizza> allPizzas);   // Displays all pizzas in a passed in Pizza vector.
void makeSalad();             // Called by placeOrder. Function lets user customize a Salad and add it to the cart.
void makeDrink();             // Called by placeOrder. Function lets user customize a Drink and add it to the cart.
void makeSide();              // Called by placeOrder. Function lets user customize a Side and add it to the cart.
void viewCart(Order& currentOrder, bool& refStillShopping, vector<Pizza>& pizzaItems); // Called by placeOrder. Gets passed currentOrder (which is a reference to curOrder), a reference to the bool variable "stillShopping" used for the food menu loop, and a reference to the vector pizzaItems. Function displays cart items, allows for order edits such as removeItem and changeDelMethod, and changeCustAddress
void removeItems();           // Called by viewCart. Function displays list of items in the cart and allow for removal.
void payForOrder(vector<Order>& refOrderQueue); // Called by mainMenu or at the end of placeOrder. Function displays the queue of unpaid orders and allows payment.
void dispAllOrders(vector<Order>& orderQueue);  // Displays all orders from the order queue.
void reports();               // (Leaving this in for now, just in case.) Called by mainMenu. Function displays different reports based on sales and inventory usage.
void support();               // Called by mainMenu. Function displays help items and ways to contact the developers (us).
void endProg(bool &refQuit);  // Called by mainMenu. Gets passed a reference to bool variable "quit". Function prompts user to quit program (quit==true) or return to main menu.
void pause();                 // Called by various functions. This is a helper function to clear the buffer and wait for "Enter".
void retryInput();            // Called by various functions. This is a helper function to prompt for input re-entry.


/// BEGINNING OF CLASSES /////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     * * *   CLASS - ORDER   * * *                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Order     // Creates an Order. Keeps track of all order related info. (Order number, customer info, price, etc.)
{
    private:
    int orderNum;                               // Order Number
    int custZip = 0;                            // Customer Zip Code
    string delMethod, custName, custAddress;    // Delivery method, Customer Name, Customer Address
    vector<Pizza> pizzaCart;   // This is a vector of Pizzas. Each time you create pizza during an order, that Pizza instance gets pushed to pizzaCart.
                                // Keep in mind, each instance of pizza has a quantity that may be greater than 1. For example, you may have
                                // 3 Medium Deep Dish pizzas with pepperoni. That instance has a quantity of 3, (so it's 3 pizzas) but is only one item in the vector.
                                // (The price will get multiplied by the quantity during checkout.)
    public:
/*
    Order()  // Class constructor. Maybe not needed. It allows default values to be set for member variables inside of a class.
    {
        int orderNum;
        int custZip = 0;
        string delMethod, custName, custAddress;
        vector<Pizza> pizzaItems;
    }
*/

// Order Class Custom Function Prototypes
// Not sure why these aren't needed, but I put them here for our reference.
/*
    void dispOrderDetails(); // Displays any available details about the order. (Order #, Delivery Method, Customer Address/Zip, Customer Name.)
    int setOrderNum();       // Returns orderNum to be displayed. It gets a new order number each time it's run, from a file, and increments it by one.
    int dispOrderNum();      // Returns int orderNum to be displayed.
    void setDelMethod();     // Prompts user for carry-out or delivery, sets delMethod to string "Carry-out" or "Delivery".
    string dispDelMethod();  // Returns string delMethod (delivery method) to be displayed.
    void setCustAddress();   // Prompts user for street address, saves input (with getline) as string custAddress.
    string dispCustAddress();// Returns string custAddress to be displayed.
    void setCustZip();       // Prompts user for their zip code, saves valid input to int custZip.
    int dispCustZip();       // Returns int custZip to be displayed.
    void setCustName();      // Prompts user for Customer name, saves input (with getline) as string custName.
    string dispCustName();   // Returns string custName to be displayed.
    void verifyOrderDetails();  // Displays the order details and asks if they would like to change any of the order details. Options will run functions: setDelMethod, setCustAddress, setCustName, or finish when everything is deemed correct.
    void savePizzasToCart(vector<Pizza> pizzaItems);  // Saves the vector of Pizzas from the temporary vector pizzaBasket to the permanent vector "pizzaCart". Called at the end of order completing.
    void dispPizzaCart();    // Displays all the pizzas in the cart for the current order.
*/

    void dispOrderDetails()     // Displays any available details about the order. (Order #, Delivery Method, Customer Address/Zip, Customer Name.)
    {                           // All of this data is collected and displayed as you go through the menu, so this function only outputs the data if it has already been set by the user.
        system(CLEAR);
        cout << " Order # " << orderNum << endl;
        if (delMethod != "")
        {
            cout << " Delivery Method: " << delMethod << endl;
        }
        if (custAddress != "")
        {
            cout << " Customer Address: " << custAddress << endl;
        }
        if (custZip != 0)
        {
            cout << " Customer Zip Code: " << custZip << endl;
        }
        if (custName != "")
        {
            cout << " Custome Name: " << custName << endl;
        }
        cout << endl;
    }

    int setOrderNum()   // Returns orderNum to be displayed. It gets a new order number each time it's run, from a file, and increments it by one.
    {                   // Because it is in a file, it will not restart at 0 when the program is restarted.
        ifstream checkFile;
        checkFile.open("order number.txt");
        if (checkFile.is_open())    // Checks to see if "order number.txt" already exists
        {
            // cout << "order number.txt opened." << endl;      // to check if file is opening
            checkFile.close();
        }
        else                        // Creates order number.txt file if it doesn't exist and populates it with 1.
        {
            cout << " order number.txt did not open. Creating new file called order number.txt with 1 as it's only internal data." << endl;
            ofstream createFile;
            createFile.open("order number.txt");
            int startingOrderNum = 0;
            createFile << startingOrderNum;
            createFile.close();
        }

        fstream openFile;
        openFile.open("order number.txt");

        openFile >> orderNum;       // Retrieves orderNum from file.
        openFile.seekg (0);         // Sets the output stream to the beginning of the file.
        openFile << ++orderNum;     // Increments orderNum and saves it back to the file.

        openFile.close();
        return orderNum;
    }

    int dispOrderNum()  // Returns int orderNum to be displayed.
    {
        return orderNum;
    }

    void setDelMethod() // Prompts user for carry-out or delivery, sets delMethod to string "Carry-out" or "Delivery".
    {
        char menuChoice = '0';
        while (menuChoice == '0')
        {
            dispOrderDetails();
            cout << " Will this order be for Carry-out or Delivery?" << endl << " 1. Carry-out" << endl << " 2. Delivery" << endl << endl;
            cin >> menuChoice;
            switch(menuChoice)
            {
                case '1':
                    delMethod = "Carry-out";
                    break;
                case '2':
                    delMethod = "Delivery";
                    break;
                default:
                    menuChoice = '0';
                    retryInput();
                    break;
            }
        }
    }

    string dispDelMethod()  // Returns string delMethod (delivery method) to be displayed.
    {
        return delMethod;
    }

    void setCustAddress()   // Prompts user for street address, saves input (with getline) as string custAddress.
    {
        string tempCustAddress = "";
        while (tempCustAddress == "")
        {
            dispOrderDetails();
            cout << " What is the customer's street address?" << endl << " (Example: 1234 Place Ave.)" << endl << endl;
            cin.ignore();                   // Clears the input buffer
            getline(cin, tempCustAddress);  // Must use getline() here, in order to get characters after a whitespace.
        }
        custAddress = tempCustAddress;
    }

    string dispCustAddress() // Returns string custAddress to be displayed.
    {
        return custAddress;
    }

    void setCustZip()   // Prompts user for their zip code, saves valid input to int custZip.
    {
        int tempCustZip = 0;
        while (tempCustZip < 1001 || tempCustZip > 99950)  // Tests to see if number is within the American Zip code range: 01001-99950 (True if it is not in range.)
        {
            dispOrderDetails();
            cout << " Customer Zip code: ";
            cin >> tempCustZip;
            if (tempCustZip < 1001 || tempCustZip > 99950)
            {
                tempCustZip = 0;
                retryInput();
            }
        }
        custZip = tempCustZip;
    }

    int dispCustZip()   // Returns int custZip to be displayed.
    {
        return custZip;
    }

    void setCustName()  // Prompts user for Customer name, saves input (with getline) as string custName.
    {
        string tempCustName = "";
        while (tempCustName == "")
        {
            dispOrderDetails();
            cout << " Customer name: ";
            cin.ignore();                  // Clears input buffer
            getline(cin, tempCustName);  // getline needed in case user inputs a space, "John Kennedy".
            while(!cin)
            {
                cout << " It's not a valid name. Please try again!\n";
                cin >> tempCustName;
            }
        }
        custName = tempCustName;
    }

    string dispCustName()   // Returns string custName to be displayed.
    {
        return custName;
    }

    void verifyOrderDetails()   // Displays the order details and asks if they would like to change any of the order details.
    {                           // Options will run functions: setDelMethod, setCustAddress, setCustName, or finish when everything is deemed correct.
        bool makeChanges = true;
        char menuChoice = '0';
        while (menuChoice == '0' || makeChanges == true)
        {
            dispOrderDetails();    // Displays any available details

            cout << " Is everything correct?" << endl << endl << " 1. Change Delivery Method" << endl << " 2. Change or Add Customer Address and Zip Code" << endl
                << " 3. Change Customer Name" << endl << " 4. Everything looks good" << endl << endl;
            cin >> menuChoice;

            system(CLEAR);
            switch (menuChoice)
            {
                case '1':
                    setDelMethod();
                    if (dispDelMethod() == "Delivery") // If delivery, get customer's address and zip code.
                    {
                        cout << endl;
                        setCustAddress();
                        cout << endl;
                        setCustZip();
                    }
                    menuChoice = '0';
                break;
                case '2':
                    cout << endl;
                    setCustAddress();
                    cout << endl;
                    setCustZip();
                    menuChoice = '0';
                break;
                case '3':
                    setCustName();
                    menuChoice = '0';
                break;
                case '4':
                    makeChanges = false;
                break;
                default:
                    menuChoice = '0';
                    makeChanges = true;
                    retryInput();
                break;
            }
        }
    }

    void savePizzasToCart(vector<Pizza> pizzaItems)  // Saves the vector of Pizzas from the temporary vector pizzaBasket to the permanent vector "pizzaCart". Called at the end of order completing.
    {
        pizzaCart = pizzaItems;
    }

    void dispPizzaCart()    // Displays all the pizzas in the cart for the current order.
    {
            dispAllPizzas(pizzaCart);
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     * * *   CLASS - PIZZA   * * *                                                              ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Pizza     // Creates a Pizza. Keeps track of size, dough type, toppings, quantity(multiple of same pizza), and price.
{
    private:
    float pPrice, pSizePrice;
    int pQuantity = 1;
    string pSize, pDough;
    vector<string> pToppings;

    public:
/*
    Pizza() // Class Constructor, maybe not needed.
    {
        float pPrice, pSizePrice;
        int pQuantity = 1;
        string pSize, pDough;
        vector<string> pToppings;
    }
*/

// Pizza Class Custom Function Prototypes
// Not sure why these aren't needed, but I put them here for our reference.
/*
    void setPizzaSize()     // Prompts user for the pizza size, sets string pSize to "Large", "Medium", or "Personal Pan", also sets the corresponding pSizePrice which is variable used elsewhere to help determine price.
    string dispPizzaSize()  // Returns string pSize (size of the pizza) to be displayed.
    void setPizzaDough()    // Prompts user for dough type, set string pDough to "Hand Tossed", "Deep Dish", or "Thin & Crispy".
    string dispPizzaDough()     // Returns string pDough (dough type) to be displayed.
    void setPizzaToppings()     // Displays a list of available toppings and allows user to add any number of toppings to the pizza.
    void dispPizzaToppings()    // Displays list of toppings on the current pizza.
    void removePizzaToppings()  // Displays list of toppings on the current pizza and allows user to remove any number of them before ending.
    void setPizzaQuantity()     // Prompts the user for quantity of this pizza, saves valid input to int pQuantity.
    int dispPizzaQuantity()    // Returns int pQuantity (quantity of this pizza) to be displayed.
    void dispPizzaDetails()     // Displays all the details of a pizza: Quantity, Size, Dough Type, and Toppings.
    void verifyPizza(Pizza& addPizza)   // Displays the pizza details including price and prompts user to confirm pizza details or: setPizzaSize, setPizzaDough, setPizzaToppings, setPizzaQuantity, or confirm.
    void setPizzaPrice()    // Calculates the price of a Pizza and sets it to float pPrice.
    float dispPizzaPrice() // Returns float pPrice (price of the pizza) to be displayed.
    void dispPizzaPriceDetails()    // Displays a breakdown readout of the cost of a Pizza. Shows the cost for the size, cost for the toppings, and the total cost after quantity has been multiplied.
*/
// Class Pizza Custom Function Prototypes

    void setPizzaSize()     // Prompts user for the pizza size, sets string pSize to "Large", "Medium", or "Personal Pan", also sets the corresponding pSizePrice which is variable used elsewhere to help determine price.
    {
        const int WIDTH_1 = 18, WIDTH_2 = 7;
        char menuChoice = '0';
        while(menuChoice == '0')
        {
            system(CLEAR);
            cout << " What size will this pizza be?" << endl << " 1. Large" << endl << " 2. Medium" << endl << " 3. Personal Pan" << endl << endl;
            cin >> menuChoice;
            switch(menuChoice)
            {
                case '1':
                    pSize = "Large";
                    pSizePrice = PRICE_L_PIZZA;
                    break;
                case '2':
                    pSize = "Medium";
                    pSizePrice = PRICE_M_PIZZA;
                    break;
                case '3':
                    pSize = "Personal Pan";
                    pSizePrice = PRICE_PP_PIZZA;
                    break;
                default:
                    menuChoice = '0';
                    retryInput();
                    break;
            }
        }
    }

    string dispPizzaSize()  // Returns string pSize (size of the pizza) to be displayed.
    {
        return pSize;
    }

    void setPizzaDough()    // Prompts user for dough type, set string pDough to "Hand Tossed", "Deep Dish", or "Thin & Crispy".
    {
        char menuChoice = '0';
        while(menuChoice == '0')
        {
            system(CLEAR);
            cout << " What type of dough will this pizza be?" << endl << " 1. Hand Tossed" << endl << " 2. Deep Dish" << endl << " 3. Thin & Crispy" << endl << endl;
            cin >> menuChoice;
            switch(menuChoice)
            {
                case '1':
                    pDough = "Hand Tossed";
                    break;
                case '2':
                    pDough = "Deep Dish";
                    break;
                case '3':
                    pDough = "Thin & Crispy";
                    break;
                default:
                    menuChoice = '0';
                    retryInput();
                    break;
            }
        }
    }

    string dispPizzaDough()     // Returns string pDough (dough type) to be displayed.
    {
        return pDough;
    }

    void setPizzaToppings()     // Displays a list of available toppings and allows user to add any number of toppings to the pizza.
    {
        const int WIDTH = 20;
        int page = 1;
        char menuChoice = '0';
        while (menuChoice != '9')
        {
            if (page == 1)
            {
                system(CLEAR);
                cout << " What toppings would you like on your " << pSize << " pizza?  ($" << PRICE_TOPPING << " per topping.)" << endl << endl;
                cout << setw(WIDTH) << left << " 1. Pepperoni" << setw(WIDTH) << "2. Ham" << endl
                    << setw(WIDTH) << " 3. Sausage" << setw(WIDTH) << "4. Bell Peppers" << endl
                    << setw(WIDTH) << " 5. Onions" << setw(WIDTH) << "6. Mushrooms" << endl << endl
                    << setw(WIDTH) << " 7. Next Page" << endl
                    << setw(WIDTH) << " 8. Remove Toppings" << endl
                    << setw(WIDTH) << " 9. Done with Toppings" << endl << endl;

                cout << " Current Toppings:" << endl;
                dispPizzaToppings();    // This display function is a bit different than our other display functions because it is a void function (it's not
                                        // returning anything to be displayed, rather, it is displaying it's contents while still inside the function and then returning nothing.
                                        // This is because our toppings are in a single vector/array and must be looped so that each toppings all may display, one at a time / one per loop.
                                        // So this will be one of our only functions (so far) that is just called like a regular function instead of "curPizzas.dispPizzaToppings".

                cout << endl;
                cin >> menuChoice;
                switch(menuChoice)
                {
                    case '1':
                        menuChoice = '0';
                        pToppings.push_back("Pepperoni");
                    break;
                    case '2':
                        menuChoice = '0';
                        pToppings.push_back("Ham");
                    break;
                    case '3':
                        menuChoice = '0';
                        pToppings.push_back("Sausage");
                    break;
                    case '4':
                        menuChoice = '0';
                        pToppings.push_back("Bell Peppers");
                    break;
                    case '5':
                        menuChoice = '0';
                        pToppings.push_back("Onions");
                    break;
                    case '6':
                        menuChoice = '0';
                        pToppings.push_back("Mushrooms");
                    break;
                    case '7':
                        menuChoice = '0';
                        page = 2;
                    break;
                    case '8':
                        menuChoice = '0';
                        if (pToppings.size() == 0)
                        {
                            system(CLEAR);
                            cout << " There are no toppings to remove. Press Enter to continue." << endl;
                            pause();
                        }
                        else
                        {
                            removePizzaToppings();
                        }
                    break;
                    case '9':
                        // Done... leave loop/menu.
                    break;
                    default:
                        menuChoice = '0';
                        retryInput();
                    break;
                }
            }

            if (page == 2)
            {
                system(CLEAR);
                cout << " What toppings would you like on your " << pSize << " pizza?" << endl << endl;
                cout << setw(WIDTH) << left << " 1. Chicken" << setw(WIDTH) << "2. Feta Cheese" << endl
                    << setw(WIDTH) << " 3. Bacon" << setw(WIDTH) << "4. Jalapeno" << endl
                    << setw(WIDTH) << " 5. Tomatoes" << setw(WIDTH) << "6. Spinach" << endl << endl
                    << setw(WIDTH) << " 7. Previous Page" << endl
                    << setw(WIDTH) << " 8. Remove Toppings" << endl
                    << setw(WIDTH) << " 9. Done with Toppings" << endl << endl;
                cout << " Current Toppings:" << endl;
                dispPizzaToppings();

                cout << endl;
                cin >> menuChoice;
                switch(menuChoice)
                {
                    case '1':
                        menuChoice = '0';
                         pToppings.push_back("Chicken");
                    break;
                    case '2':
                        menuChoice = '0';
                        pToppings.push_back("Feta Cheese");
                    break;
                    case '3':
                        menuChoice = '0';
                        pToppings.push_back("Bacon");
                    break;
                    case '4':
                        menuChoice = '0';
                        pToppings.push_back("Jalapeno");
                    break;
                    case '5':
                        menuChoice = '0';
                        pToppings.push_back("Tomatoes");
                    break;
                    case '6':
                        menuChoice = '0';
                        pToppings.push_back("Spinach");
                    break;
                    case '7':
                        menuChoice = '0';
                        page = 1;
                    break;
                    case '8':
                        menuChoice = '0';
                        removePizzaToppings();
                    break;
                    case '9':

                    break;
                    default:
                        menuChoice = '0';
                        retryInput();
                    break;
                }
            }
        }
    }

    void dispPizzaToppings()    // Displays list of toppings on the current pizza.
    {
        if (pToppings.size() == 0)
        {
            cout << " Cheese only" << endl;
        }
        for (int index = 0; index < pToppings.size(); index++)  // Loops through the toppings vector, displaying one each time the loop iterates.
        {
            cout << " " << pToppings[index] << endl;       // To access an element in a vector, name the variable and use brackets to send what number in the list
                                                   // of elements like this: pToppings[3]. Since "index" iterates by 1 each loop, the elements are accessed
                                                   // in order, from 0 until it reaches the last one.
        }
    }

    void removePizzaToppings()  // Displays list of toppings on the current pizza and allows user to remove any number of them before ending.
    {
        char menuChoice = '0';
        while (menuChoice == '0')
        {
            system(CLEAR);
            cout << " What topping would you like to remove?" << endl;
            for (int index = 0; index < pToppings.size(); index++)
            {
                cout << " " << index + 1 << ". " << pToppings[index] << endl;
            }
            cout << endl << " 9. Done removing toppings" << endl << endl;

            cin >> menuChoice;
            if (menuChoice > pToppings.size() + 48 && menuChoice != '9') // If choice is greater than the number of toppings retry input. (ex. You have 3 toppings
            {                                      // able to be removed, but you choose option 6, which exists as an switch option,
                menuChoice = '0';                  // but you don't have 6 toppings, so it would end up just removing the last topping.
                retryInput();                      // This code stops that.) The "+ 48" changes menuChoice (which is a char) to an int, due
            }                                      // to the ASCII table. The "&& menuChoice != '9'" is needed in case user is done, which is option 9.
            switch(menuChoice)
            {
                case '0':
                    // Needed in case above "if" statement is true, retry input doesn't get run again on the default case.
                break;
                case '1':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin());
                break;
                case '2':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+1);
                break;
                case '3':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+2);
                break;
                case '4':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+3);
                break;
                case '5':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+4);
                break;
                case '6':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+5);
                break;
                case '7':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+6);
                break;
                case '8':
                    menuChoice = '0';
                    pToppings.erase(pToppings.begin()+7);
                break;
                case '9':
                    // Done. Loop finishes and function ends.
                break;
                default:
                    menuChoice = '0';
                    retryInput();
                break;
            }
        }
    }

    void setPizzaQuantity()     // Prompts the user for quantity of this pizza, saves valid input to int pQuantity.
    {
        int menuChoice = 0;
        while(menuChoice == 0)
        {
            system(CLEAR);
            cout << " Your pizza: " << endl << endl;
            dispPizzaDetails();
            cout << endl << " How many pizzas like this would you like? Enter quantity:" << endl << endl;
            cin >> menuChoice;

            if (menuChoice < 1 || menuChoice > 20)
            {
                menuChoice = 0;
                system(CLEAR);
                cout << " Sorry, 20 pizzas is the largest order we can take at one time." << endl << " Please press Enter, then input a number between 1-30." << endl;
                pause();
            }
        }
        pQuantity = menuChoice;

    }

    int dispPizzaQuantity()    // Returns int pQuantity (quantity of this pizza) to be displayed.
    {
        return pQuantity;
    }

    void dispPizzaDetails()     // Displays all the details of a pizza: Quantity, Size, Dough Type, and Toppings.
    {
        cout << " " << pQuantity << " " << pSize << ", " << pDough << " pizza";
        if (pQuantity > 1)
        {
            cout << "s";
        }
        cout << endl;
        dispPizzaToppings();
    }

    void verifyPizza(Pizza& addPizza)   // Displays the pizza details including price and prompts user to confirm pizza details or: setPizzaSize, setPizzaDough, setPizzaToppings, setPizzaQuantity, or confirm.
    {
        char menuChoice = '0';
        while(menuChoice == '0')
        {
            system(CLEAR);
            cout << " Your pizza: " << endl << endl;
            dispPizzaDetails();

            setPizzaPrice();
            cout << endl << "Price for this pizza: " << endl;
            dispPizzaPriceDetails();
            cout << endl << " How does this look?" << endl << " 1. Change Size" << endl << " 2. Change Dough" << endl << " 3. Modify Toppings" << endl <<
                                 " 4. Change Quantity" << endl << " 5. Looks good, Add to Cart" << endl << endl;
            cin >> menuChoice;
            switch(menuChoice)
            {
                case '1':
                    menuChoice = '0';
                    addPizza.setPizzaSize();
                break;
                case '2':
                    menuChoice = '0';
                    addPizza.setPizzaDough();
                break;
                case '3':
                    menuChoice = '0';
                    addPizza.setPizzaToppings();
                break;
                case '4':
                    menuChoice = '0';
                    addPizza.setPizzaQuantity();
                case '5':

                    // Done, loop stops.
                break;
                default:
                    menuChoice = '0';
                    retryInput();
                break;
            }
        }
    }

    void setPizzaPrice()    // Calculates the price of a Pizza and sets it to float pPrice.
    {
        pPrice = 0.0;
        pPrice += pSizePrice;
        if (pToppings.size() > 0)
        {
            pPrice += (PRICE_TOPPING * pToppings.size());
        }
    }

    float dispPizzaPrice() // Returns float pPrice (price of the pizza) to be displayed.
    {
        return pPrice;
    }

    void dispPizzaPriceDetails()    // Displays a breakdown readout of the cost of a Pizza. Shows the cost for the size, cost for the toppings, and the total cost after quantity has been multiplied.
    {
        cout << " " << pSize << " = $" << pSizePrice << " +" << endl <<
                " " << pToppings.size() << " toppings @ $" << PRICE_TOPPING << " each = $" << PRICE_TOPPING * pToppings.size() << endl <<
                " Subtotal = $" << pPrice << " x " << pQuantity << " = $" << (pPrice * pQuantity) << endl;
    }
};

/// END OF CLASSES ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// BEGINNING OF FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           * * *   MAIN   * * *                                                                 ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUNCTION main - runs mainMenu() function until bool value "quit" is true.
int main()
{
    bool quit = false;
    while (!quit)
    {
        mainMenu(quit);
    }
    system(CLEAR);
    cout << " Thank you for using RMS." << endl << " For software support, contact Ryan, Savannah, and John @ Reggett@webdmc.delmar.edu" << endl;
    return 0;
}

// FUNCTION mainMenu - Displays and runs main menu functions
void mainMenu(bool &refQuit)
{
    vector<Order> orderQueue;
    char menuChoice = '0';
    while (refQuit == false)
    {
        system(CLEAR);
        cout << "  - RMS Main Menu - " << endl << " 1. Place Order" << endl << " 2. Pay for order" << endl << endl << " 8. Support / Dev Team" << endl << " 9. End program" << endl << endl;
        cin >> menuChoice;
        switch(menuChoice)
        {
            case '1':
                orderQueue.push_back(placeOrder());
                menuChoice = '0';
                break;
            case '2':
                if (orderQueue.size() == 0)
                {
                    system(CLEAR);
                    cout << endl << " There are no unpaid orders! Press Enter to continue." << endl;
                    pause();
                }
                else
                {
                    payForOrder(orderQueue);
                }
                menuChoice = '0';
                break;
            case '3':
                reports();
                menuChoice = '0';
                break;
            case '8':
                support();
                menuChoice = '0';
                break;
            case '9':
                endProg(refQuit);
                menuChoice = '0';
                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

// FUNCTION placeOrder - Starts the process of completing an order
Order placeOrder()
{
    system(CLEAR);
    Order curOrder;  // New instance of the class Order - ("curOrder" stands for Current Order)

    curOrder.setOrderNum();      // Grabs last order number from file, increments by one and sets it to the new order.
    curOrder.setDelMethod();        // Asks for Delivery Method

    if (curOrder.dispDelMethod() == "Delivery") // If order is for Delivery, get customer's address and zip code.
    {
        curOrder.setCustAddress();      // Asks for Customer Address
        curOrder.setCustZip();          // Asks for Customer Zip code
    }

    curOrder.setCustName();         // Asks for Customer Name
    curOrder.verifyOrderDetails();  // Asks user if order info is correct and allows for change/correction.

    foodMenu(curOrder);     // Displays the 4 food options, "view cart", and "remove items", then runs the appropriate function. Ie. makePizza(), viewCart(), etc.

    return curOrder;
}

void foodMenu(Order& refCurrentOrder)
{
    vector<Pizza> pizzaBasket;
    char menuChoice = '0';
    bool stillShopping = true; // This is used to continue the Food menu until user is ready to checkout.
    while (menuChoice == '0' || stillShopping == true)
    {
        system(CLEAR);
        cout << "  - Food Menu - " << endl;
        cout << " 1. Pizza" << endl << " 2. Salad" << endl << " 3. Drinks" << endl << " 4. Sides" << endl << endl << " 5. View Cart / Checkout" << endl << endl << " 9. Cancel order" << endl << endl;
        cin >> menuChoice;
        switch(menuChoice)
        {
            case '1':
                pizzaBasket.push_back(makePizza());
                break;
            case '2':
                makeSalad();
                break;
            case '3':
                makeDrink();
                break;
            case '4':
                makeSide();
                break;
            case '5':
                viewCart(refCurrentOrder, stillShopping, pizzaBasket);
                break;
            case '9':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

/////////////////////////////////////
/// BEGINNING OF "MAKE" FUNCTIONS ///
/////////////////////////////////////
Pizza makePizza()          // This function returns a variable of data type/Class Pizza.
{
    Pizza curPizza;        // This line declares a variable/instance of the Pizza data type/class. This variable or instance is called curPizza (for current pizza).
                           // When working with this instance, you will access it's properties (private variables) by calling functions that are specific to the Pizza class.
                           // For instance, our first call will be "curPizza.setPizzaSize()", where "curPizza" is the instance of the Pizza class,
                           // and ".setPizzaSize()" calls the custom function inside of the Pizza class and does any operations on the current instance,
                           // in this case, curPizza. Inside the setPizzaSize() function, the user is prompted for the size and then the private variable
                           // "pSize" will be set to the user's choice and then the function will end and the next line will continue.

    cout << "  - Pizza Menu - " << endl;
    curPizza.setPizzaSize();

    curPizza.setPizzaDough();

    curPizza.setPizzaToppings();

    curPizza.setPizzaQuantity();

    curPizza.verifyPizza(curPizza);

    return curPizza;
}

void dispAllPizzas(vector <Pizza> allPizzas)  // Displays all the pizzas in a vector
{
    Pizza curPizza;
    for (int index = 0; index < allPizzas.size(); index++)
    {
        curPizza = allPizzas[index];
        cout << " Item " << index + 1 << ":" << endl;
        curPizza.dispPizzaDetails();
        cout << endl;
    }
}

void makeSalad()
{
    system(CLEAR);
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        cout << " This is the makeSalad() function. It has not been written yet." << endl << endl;
        cout << " This is where the first menu will be. " << endl << " 1. Choice A" << endl << " 2. Choice B" << endl << " 3. Choice C" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':

                break;
            case '2':

                break;
            case '3':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

void makeDrink()
{
    system(CLEAR);
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        cout << " This is the makeDrink() function. It has not been written yet." << endl << endl;
        cout << " This is where the first menu will be. " << endl << " 1. Choice A" << endl << " 2. Choice B" << endl << " 3. Choice C" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':

                break;
            case '2':

                break;
            case '3':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

void makeSide()
{
    system(CLEAR);
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        cout << " This is the makeSide() function. It has not been written yet." << endl << endl;
        cout << " This is where the first menu will be. " << endl << " 1. Choice A" << endl << " 2. Choice B" << endl << " 3. Choice C" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':

                break;
            case '2':

                break;
            case '3':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}
///////////////////////////////
/// END OF "MAKE" FUNCTIONS ///
///////////////////////////////

void viewCart(Order& currentOrder, bool& refStillShopping, vector<Pizza>& pizzaCart)
{
    // Display cart items and order details (Order number, customer name, delivery method, food items, etc.
    // 1. Continue Shopping
    // 2. Remove a food item
    // 3. Change order details
    // 4. Complete Order
    char menuChoice = '0';
    while (menuChoice == '0')
    {

        system(CLEAR);
        currentOrder.dispOrderDetails();
        cout << " Items on your order:" << endl << endl;
        dispAllPizzas(pizzaCart);
        cout << " 1. Continue Shopping" << endl << " 2. Remove an item" << endl << " 3. Change order details" << endl << " 4. Complete Order" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':
                // Continue shopping - nothing happens, loop cycles, we get taken back to food menu.
                break;
            case '2':
                removeItems();
                break;
            case '3':
                currentOrder.verifyOrderDetails();
                break;
            case '4':
                currentOrder.savePizzasToCart(pizzaCart);
                refStillShopping = false;
                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }

    }
}

/// EMPTY FUNCTION ///
void removeItems()
{
    system(CLEAR);
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        cout << " This is the removeItems() function. It has not been written yet." << endl << endl;
        cout << " This is where the first menu will be. " << endl << " 1. Choice A" << endl << " 2. Choice B" << endl << " 3. Choice C" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':

                break;
            case '2':

                break;
            case '3':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

// FUNCTION payForOrder - Displays the queue of placed orders, will allow user to choose one and make a payment.
void payForOrder(vector<Order>& refOrderQueue)
{
    system(CLEAR);
    int menuChoice = 0;
    while (menuChoice != (refOrderQueue.size() + 1) )
    {
        cout << " Which order would you like to pay for?" << endl << endl;
        dispAllOrders(refOrderQueue);
        cout << " " << (refOrderQueue.size() + 1) << ". Back to Main Menu" << endl;
        cin >> menuChoice;
        if (menuChoice < 1 || menuChoice > (refOrderQueue.size() +1)) // Input validation: If choice is less than one or greater than the number of orders plus one. (Plus one exits).
        {
            menuChoice = 0;
            retryInput();
        }
        else if (menuChoice == (refOrderQueue.size() + 1) )  // If choice is to go back to main menu.
        {
            // Loop ends, back to Main Menu.
        }
        else        // Chooses the order from the vector using the valid menuChoice input. Displays...
        {
            Order curOrder;
            curOrder = refOrderQueue[menuChoice];
            curOrder.dispOrderDetails();
            curOrder.dispPizzaCart();
            pause();
        }


    }
}

void dispAllOrders(vector<Order>& orderQueue)
{
        Order curOrder;
        for (int index = 0; index < orderQueue.size(); index++)
        {
            curOrder = orderQueue[index];
            cout << " " << index + 1 << ". Order #" << curOrder.dispOrderNum() << ", " << curOrder.dispDelMethod() << "  Name: " << curOrder.dispCustName() << endl;
        }
        cout << endl;
}

/// EMPTY FUNCTION ///
// FUNCTION reports - Displays stored data of sales
void reports()
{
    system(CLEAR);
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        cout << " SECRET MENU!!!" << endl << endl << " This is the reports() function. It has not been written yet." << endl << endl;
        cout << " This is where the first menu will be. " << endl << " 1. Choice A" << endl << " 2. Choice B" << endl << " 3. Choice C" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':

                break;
            case '2':

                break;
            case '3':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

/// EMPTY FUNCTION ///
// FUNCTION support - Displays support information
void support()
{
    system(CLEAR);
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        cout << " This is the support() function. It has not been written yet." << endl << endl;
        cout << " This is where the first menu will be. " << endl << " 1. Choice A" << endl << " 2. Choice B" << endl << " 3. Choice C" << endl << endl;
        cin >> menuChoice;

        switch(menuChoice)
        {
            case '1':

                break;
            case '2':

                break;
            case '3':

                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

// FUNCTION endProg - Asks the user if they want to quit or return to the main menu.
void endProg(bool &refQuit)
{
    char menuChoice = '0';
    while (menuChoice == '0')
    {
        system(CLEAR);
        cout << " All done for the day?" << endl << " 1. Oops, I'm not done. Return to main menu." << endl << " 2. Yes, lets go home." << endl;
        cin >> menuChoice;
        switch(menuChoice)
        {
            case '1':
                refQuit = false;
                break;
            case '2':
                refQuit = true;
                break;
            default:
                menuChoice = '0';
                retryInput();
                break;
        }
    }
}

// FUNCTION pause() - Quick function for clearing the buffer and waiting for enter to be pressed.
void pause()
{
    cin.ignore();
    cin.get();
}

// FUNCTION retryInput() - Quick function for clearing screen and prompting for new input
void retryInput()
{
    system(CLEAR);
    cout << " Sorry, that's not a valid choice. Please press 'Enter', then try again." << endl;
    pause();
    system(CLEAR);
}

/// END OF FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////////////////////////////
