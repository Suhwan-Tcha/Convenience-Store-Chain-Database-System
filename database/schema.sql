DROP VIEW IF EXISTS Product_Stock;
DROP VIEW IF EXISTS Store_Sales;
DROP VIEW IF EXISTS Store_Revenue;
DROP VIEW IF EXISTS Vendor_Supply;
DROP VIEW IF EXISTS Inventory_Alert;
DROP VIEW IF EXISTS VVIP_Transactions;
DROP VIEW IF EXISTS Store_Variety;

DROP TABLE IF EXISTS store_phone;
DROP TABLE IF EXISTS vendor_contact;
DROP TABLE IF EXISTS product_category;
DROP TABLE IF EXISTS customer_phone;
DROP TABLE IF EXISTS customer_email;
DROP TABLE IF EXISTS store_product;
DROP TABLE IF EXISTS transaction_item;
DROP TABLE IF EXISTS sales_transaction;
DROP TABLE IF EXISTS product;
DROP TABLE IF EXISTS vendor;
DROP TABLE IF EXISTS customer;
DROP TABLE IF EXISTS store;


CREATE TABLE store (
  store_id INT,
  name VARCHAR(100) NOT NULL,
  address VARCHAR(255) NOT NULL,
  open_time TIME,
  close_time TIME,
  ownership_type ENUM('Franchise', 'Corporate'),
  PRIMARY KEY (store_id),
  CHECK (open_time < close_time)
);

CREATE TABLE store_phone (
  store_id INT,
  phone VARCHAR(20),
  PRIMARY KEY (store_id, phone),
  FOREIGN KEY (store_id) REFERENCES store(store_id)
);

CREATE TABLE vendor (
  vendor_id INT,
  name VARCHAR(100) NOT NULL,
  PRIMARY KEY (vendor_id)
);

CREATE TABLE vendor_contact (
  vendor_id INT,
  contact_info VARCHAR(100),
  PRIMARY KEY (vendor_id, contact_info),
  FOREIGN KEY (vendor_id) REFERENCES vendor(vendor_id)
);

CREATE TABLE product (
  product_upc VARCHAR(13),
  name VARCHAR(100) NOT NULL,
  brand VARCHAR(100) NOT NULL,
  package_type VARCHAR(50) NOT NULL,
  size VARCHAR(50) NOT NULL,
  price DECIMAL(10,2) NOT NULL CHECK (price > 0),
  vendor_id INT NOT NULL,
  PRIMARY KEY (product_upc),
  FOREIGN KEY (vendor_id) REFERENCES vendor(vendor_id)
);

CREATE TABLE product_category (
  product_upc VARCHAR(13),
  category VARCHAR(50),
  PRIMARY KEY (product_upc, category),
  FOREIGN KEY (product_upc) REFERENCES product(product_upc)
);


CREATE TABLE customer (
  customer_id INT,
  first_name VARCHAR(50) NOT NULL,
  last_name VARCHAR(50) NOT NULL,
  loyalty_status ENUM('VVIP', 'VIP', 'Regular') NOT NULL,
  PRIMARY KEY (customer_id)
);

CREATE TABLE customer_phone (
  customer_id INT,
  phone VARCHAR(20),
  PRIMARY KEY (customer_id, phone),
  FOREIGN KEY (customer_id) REFERENCES customer(customer_id)
);

CREATE TABLE customer_email (
  customer_id INT,
  email VARCHAR(100),
  PRIMARY KEY (customer_id, email),
  FOREIGN KEY (customer_id) REFERENCES customer(customer_id)
);


CREATE TABLE sales_transaction (
  transaction_id INT,
  store_id INT,
  vendor_id INT,
  customer_id INT,
  transaction_type ENUM('In_Store', 'Online_pickup', 'Online_delivery') NOT NULL,
  date_time DATETIME,
  payment_method VARCHAR(50),
  PRIMARY KEY (transaction_id),
  FOREIGN KEY (store_id) REFERENCES store(store_id),
  FOREIGN KEY (vendor_id) REFERENCES vendor(vendor_id),
  FOREIGN KEY (customer_id) REFERENCES customer(customer_id),
  CHECK (
    (store_id IS NOT NULL AND vendor_id IS NULL)
    OR (store_id IS NULL AND vendor_id IS NOT NULL)
  )
);


CREATE TABLE store_product (
  store_id INT,
  product_upc VARCHAR(13),
  inventory_level INT NOT NULL CHECK (inventory_level >= 0),
  reorder_threshold INT NOT NULL CHECK (reorder_threshold >= 0),
  reorder_quantity INT NOT NULL CHECK (reorder_quantity > 0),
  PRIMARY KEY (store_id, product_upc),
  FOREIGN KEY (store_id) REFERENCES store(store_id),
  FOREIGN KEY (product_upc) REFERENCES product(product_upc)
);


CREATE TABLE transaction_item (
  transaction_id INT,
  product_upc VARCHAR(13),
  quantity INT NOT NULL CHECK (quantity >= 1),
  PRIMARY KEY (transaction_id, product_upc),
  FOREIGN KEY (transaction_id) REFERENCES sales_transaction(transaction_id),
  FOREIGN KEY (product_upc) REFERENCES product(product_upc)
);

-- view
CREATE VIEW Product_Stock AS
SELECT s.name AS store_name, p.product_upc, p.name AS product_name, p.brand, sp.inventory_level
FROM store s
JOIN store_product sp ON s.store_id = sp.store_id
JOIN product p ON sp.product_upc = p.product_upc;

CREATE VIEW Store_Sales AS
SELECT s.store_id, s.name AS store_name, p.product_upc, p.name AS product_name,
       st.date_time, SUM(ti.quantity) AS total_sold
FROM store s
JOIN sales_transaction st ON s.store_id = st.store_id
JOIN transaction_item ti ON st.transaction_id = ti.transaction_id
JOIN product p ON ti.product_upc = p.product_upc
GROUP BY s.store_id, s.name, p.product_upc, p.name, st.date_time;


CREATE VIEW Store_Revenue AS
SELECT s.store_id, s.name AS store_name, st.date_time, SUM(ti.quantity * p.price) AS total_revenue
FROM store s
JOIN sales_transaction st ON s.store_id = st.store_id
JOIN transaction_item ti ON st.transaction_id = ti.transaction_id
JOIN product p ON ti.product_upc = p.product_upc
GROUP BY s.store_id, s.name, st.date_time;


CREATE VIEW Vendor_Supply AS
SELECT v.name AS vendor_name, COUNT(DISTINCT p.product_upc) AS product_types,
       SUM(sp.inventory_level) AS total_restock
FROM vendor v
JOIN product p ON v.vendor_id = p.vendor_id
JOIN store_product sp ON p.product_upc = sp.product_upc
GROUP BY v.vendor_id, v.name;


CREATE VIEW Inventory_Alert AS
SELECT s.name AS store_name, p.name AS product_name, sp.inventory_level, sp.reorder_threshold
FROM store s
JOIN store_product sp ON s.store_id = sp.store_id
JOIN product p ON sp.product_upc = p.product_upc
WHERE sp.inventory_level < sp.reorder_threshold;


CREATE VIEW VVIP_Transactions AS
SELECT ti.transaction_id, ti.product_upc, ti.quantity
FROM transaction_item ti
JOIN sales_transaction st ON ti.transaction_id = st.transaction_id
JOIN customer c ON st.customer_id = c.customer_id
WHERE c.loyalty_status = 'VVIP';


CREATE VIEW Store_Variety AS
SELECT s.name AS store_name, s.ownership_type, COUNT(DISTINCT sp.product_upc) AS product_variety
FROM store s
JOIN store_product sp ON s.store_id = sp.store_id
GROUP BY s.store_id, s.name, s.ownership_type;
