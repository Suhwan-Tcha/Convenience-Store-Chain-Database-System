#include <iostream>
#include <mysql.h>
#include <string>
using namespace std;

MYSQL *conn;


void displayMenu() {
    cout << "----------- SELECT QUERY TYPES -----------\n\n";
    cout << "\t1. TYPE 1\n";
    cout << "\t2. TYPE 2\n";
    cout << "\t3. TYPE 3\n";
    cout << "\t4. TYPE 4\n";
    cout << "\t5. TYPE 5\n";
    cout << "\t6. TYPE 6\n";
    cout << "\t7. TYPE 7\n";
    cout << "\t0. QUIT\n\n";
    cout << "SELECT: ";
}


string query1() {
    cout << "------- TYPE 1 -------\n";
    cout << "** Which stores currently carry a certain product (by UPC, name, or brand), and how much inventory do they have? **\n";
    cout << "Enter product identifier (UPC, name or brand): ";
    string input;
    cin >> input;
    return "SELECT store_name, product_name, inventory_level FROM Product_Stock "
           "WHERE product_upc = '" + input + "' OR product_name = '" + input + "' OR brand = '" + input + "'";
}


string query2() {
    cout << "------- TYPE 2 -------\n";
    cout << "** Which products have the highest sales volume in each store over the past month? **\n";
    return "SELECT store_name, product_name, total_sold FROM Store_Sales s1 "
           "WHERE date_time BETWEEN '2025-05-01 00:00:00' AND '2025-05-31 23:59:59' AND total_sold = ("
           "SELECT MAX(total_sold) FROM Store_Sales s2 WHERE s1.store_id = s2.store_id "
           "AND date_time BETWEEN '2025-05-01 00:00:00' AND '2025-05-31 23:59:59')";
}


string query3() {
    cout << "------- TYPE 3 -------\n";
    cout << "** Which store has generated the highest overall revenue this quarter? **\n";
    return "SELECT store_name, SUM(total_revenue) AS total FROM Store_Revenue "
           "WHERE date_time BETWEEN '2025-04-01 00:00:00' AND '2025-06-30 23:59:59' "
           "GROUP BY store_name ORDER BY total DESC LIMIT 1";
}


string query4() {
    cout << "------- TYPE 4 -------\n";
    cout << "** Which vendor supplies the most products across the chain, and how many total restocking? **\n";
    return "SELECT vendor_name, product_types, total_restock FROM Vendor_Supply ORDER BY product_types DESC LIMIT 1";
}


string query5() {
    cout << "------- TYPE 5 -------\n";
    cout << "** Which products in each store are below the reorder threshold and need restocking? **\n";
    return "SELECT * FROM Inventory_Alert ORDER BY store_name, product_name";
}


string query6() {
    cout << "------- TYPE 6 -------\n";
    cout << "** List the top 3 items that loyalty program customers typically purchase with .... **\n";
    cout << "Enter a product name: ";
    string input;
    cin >> input;

    return "WITH Target AS ("
           "SELECT DISTINCT transaction_id FROM VVIP_Transactions v "
           "JOIN product p ON v.product_upc = p.product_upc WHERE p.name LIKE '%" + input + "%') "
           "SELECT p.name, SUM(v.quantity) AS total_quantity FROM VVIP_Transactions v "
           "JOIN Target t ON v.transaction_id = t.transaction_id "
           "JOIN product p ON v.product_upc = p.product_upc WHERE p.name NOT LIKE '%" + input + "%' "
           "GROUP BY p.name ORDER BY total_quantity DESC LIMIT 3";
}


string query7() {
    cout << "------- TYPE 7 -------\n";
    cout << "** Among franchise-owned stores, which one offers the widest variety of products, and how does that compare to corporate-owned stores? **\n";
    return "WITH FranchiseMax AS ("
           "SELECT store_name, product_variety FROM Store_Variety WHERE ownership_type = 'Franchise' ORDER BY product_variety DESC LIMIT 1), "
           "CorporateAvg AS ("
           "SELECT 'Corporate Average' AS store_name, AVG(product_variety) AS product_variety FROM Store_Variety WHERE ownership_type = 'Corporate') "
           "SELECT * FROM FranchiseMax UNION ALL SELECT * FROM CorporateAvg";
}


void exitProgram() {
    mysql_close(conn);
    exit(0);
}


void executeQuery(const string &query) {
    if (mysql_query(conn, query.c_str())) {
        cerr << "SELECT failed. Error: " << mysql_error(conn) << "\n";
        return;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (res == nullptr) {
        cerr << "mysql_store_result() failed. Error: " << mysql_error(conn) << "\n";
        return;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_FIELD *fields = mysql_fetch_fields(res);

    for (int i = 0; i < num_fields; i++) {
        cout << fields[i].name << "\t";
    }
    cout << "\n";

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        for (int i = 0; i < num_fields; i++) {
            cout << (row[i] ? row[i] : "NULL") << "\t";
        }
        cout << "\n";
    }

    mysql_free_result(res);
}

int main() {
    const char *server = "localhost";
    const char *user = "root";
    const char *password = "1234";
    const char *database = "store";

    conn = mysql_init(nullptr);
    if (conn == nullptr) {
        cerr << "mysql_init() failed\n";
        return 1;
    }

    mysql_ssl_mode sslmode = SSL_MODE_DISABLED;
    if (mysql_options(conn, MYSQL_OPT_SSL_MODE, &sslmode)) {
        cerr << "mysql_options() failed: " << mysql_error(conn) << "\n";
        mysql_close(conn);
        return 1;
    }

    if (mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0) == nullptr) {
        cerr << "mysql_real_connect() failed: " << mysql_error(conn) << "\n";
        mysql_close(conn);
        return 1;
    }

    // Loyalty Status 업데이트
    const char *update_vip = "UPDATE customer c "
                             "JOIN ("
                                 "SELECT st.customer_id, SUM(p.price * ti.quantity) AS total_spent "
                                 "FROM transaction_item ti "
                                 "JOIN product p ON ti.product_upc = p.product_upc "
                                 "JOIN sales_transaction st ON ti.transaction_id = st.transaction_id "
                                 "GROUP BY st.customer_id "
                                 "HAVING total_spent >= 10 AND total_spent < 50"
                             ") AS totals ON c.customer_id = totals.customer_id "
                             "SET c.loyalty_status = 'VIP'";

    mysql_query(conn, update_vip);

    const char *update_vvip = "UPDATE customer c "
                              "JOIN ("
                                  "SELECT st.customer_id, SUM(p.price * ti.quantity) AS total_spent "
                                  "FROM transaction_item ti "
                                  "JOIN product p ON ti.product_upc = p.product_upc "
                                  "JOIN sales_transaction st ON ti.transaction_id = st.transaction_id "
                                  "GROUP BY st.customer_id "
                                  "HAVING total_spent >= 50"
                              ") AS totals ON c.customer_id = totals.customer_id "
                              "SET c.loyalty_status = 'VVIP'";
    
    mysql_query(conn, update_vvip);

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;
        switch (choice) {
            case 1: executeQuery(query1()); break;
            case 2: executeQuery(query2()); break;
            case 3: executeQuery(query3()); break;
            case 4: executeQuery(query4()); break;
            case 5: executeQuery(query5()); break;
            case 6: executeQuery(query6()); break;
            case 7: executeQuery(query7()); break;
            case 0: exitProgram(); break;
            default: cout << "Invalid choice. Try again.\n";
        }
    }

    mysql_close(conn);
    return 0;
}
