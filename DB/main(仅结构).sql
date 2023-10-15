/*
 Navicat Premium Data Transfer

 Source Server         : data
 Source Server Type    : SQLite
 Source Server Version : 3030001
 Source Schema         : main

 Target Server Type    : SQLite
 Target Server Version : 3030001
 File Encoding         : 65001

 Date: 08/06/2023 18:18:16
*/

PRAGMA foreign_keys = false;

-- ----------------------------
-- Table structure for apply
-- ----------------------------
DROP TABLE IF EXISTS "apply";
CREATE TABLE "apply" (
  "id" integer PRIMARY KEY AUTOINCREMENT,
  "reader_id" INT NOT NULL,
  "reader_name" varchar(50) NOT NULL,
  "book_id" int NOT NULL,
  "book_name" varchar(50) NOT NULL,
  "apply_date" timestamp NOT NULL DEFAULT CURRENT_DATE,
  "apply_day" varchar(20) NOT NULL,
  "return_date" timestamp DEFAULT NULL,
  "apply_type" varchar(20) DEFAULT NULL,
  "book_status" varchar(255) DEFAULT NULL,
  "renew_day" varchar(20) DEFAULT '无',
  "reader_identify" varchar(10) NOT NULL
);

-- ----------------------------
-- Table structure for books
-- ----------------------------
DROP TABLE IF EXISTS "books";
CREATE TABLE "books" (
  "id" integer PRIMARY KEY AUTOINCREMENT,
  "book_id" int NOT NULL,
  "book_name" varchar(50) NOT NULL,
  "book_author" varchar(50) NOT NULL,
  "book_pub" varchar(50) DEFAULT NULL,
  "book_pub_date" varchar(20) DEFAULT NULL,
  "book_store" varchar(5) NOT NULL,
  "book_total" varchar(5) NOT NULL,
  "book_borrowable" varchar(5) NOT NULL,
  "book_type" varchar(10) DEFAULT '其他',
  "book_price" varchar(10) DEFAULT '未知',
  "book_entry_date" timestamp NOT NULL DEFAULT CURRENT_DATE,
  "book_image" BLOB DEFAULT NULL,
  "book_intro" TEXT DEFAULT '暂无简介~',
  UNIQUE ("book_id" ASC)
);

-- ----------------------------
-- Table structure for id_type
-- ----------------------------
DROP TABLE IF EXISTS "id_type";
CREATE TABLE "id_type" (
  "id" integer PRIMARY KEY AUTOINCREMENT,
  "usr_id" int NOT NULL,
  "usr_type" varchar(10) NOT NULL,
  "usr_state" varchar(2),
  UNIQUE ("usr_id" ASC)
);

-- ----------------------------
-- Table structure for managers
-- ----------------------------
DROP TABLE IF EXISTS "managers";
CREATE TABLE "managers" (
  "id" integer PRIMARY KEY AUTOINCREMENT,
  "man_id" int NOT NULL,
  "man_password" varchar(255) NOT NULL,
  "man_name" varchar(255) NOT NULL,
  "man_sex" char(2) DEFAULT NULL,
  "man_identify" varchar(10) NOT NULL,
  "man_salary" decimal(6,2) DEFAULT 0,
  "man_phone" varchar(50) DEFAULT NULL,
  "man_entry_date" timestamp NOT NULL DEFAULT CURRENT_DATE,
  "man_image" BLOB DEFAULT NULL,
  UNIQUE ("man_id" ASC)
);

-- ----------------------------
-- Table structure for new_ids
-- ----------------------------
DROP TABLE IF EXISTS "new_ids";
CREATE TABLE "new_ids" (
  "id" integer PRIMARY KEY AUTOINCREMENT,
  "new_id" int NOT NULL,
  UNIQUE ("new_id" ASC)
);

-- ----------------------------
-- Table structure for records
-- ----------------------------
DROP TABLE IF EXISTS "records";
CREATE TABLE "records" (
  "id" integer PRIMARY KEY AUTOINCREMENT,
  "reader_id" int NOT NULL,
  "reader_name" varchar(50) NOT NULL,
  "book_id" int NOT NULL,
  "book_name" varchar(50) NOT NULL,
  "borrow_date" timestamp NOT NULL DEFAULT CURRENT_DATE,
  "holdTime" varchar(20) NOT NULL,
  "remainder" varchar(20) NOT NULL,
  "is_return" varchar(1) NOT NULL DEFAULT '否',
  "return_date" timestamp DEFAULT NULL,
  "book_status" varchar(20) DEFAULT NULL,
  "history_renew_day" varchar(20) DEFAULT '无',
  "is_overdue" varchar(5) NOT NULL DEFAULT '否',
  "reader_identify" varchar(10) NOT NULL
);

-- ----------------------------
-- Table structure for sqlite_sequence
-- ----------------------------
DROP TABLE IF EXISTS "sqlite_sequence";
CREATE TABLE "sqlite_sequence" (
  "name" ,
  "seq" 
);

-- ----------------------------
-- Table structure for users
-- ----------------------------
DROP TABLE IF EXISTS "users";
CREATE TABLE "users" (
  "id" integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "usr_id" int NOT NULL,
  "usr_password" varchar(255) NOT NULL,
  "usr_name" varchar(255) NOT NULL,
  "usr_sex" char(2) DEFAULT NULL,
  "usr_identify" varchar(10) NOT NULL,
  "usr_overdue_num" varchar(10) DEFAULT '0次',
  "usr_loss_num" varchar(10) DEFAULT '0次',
  "usr_college" varchar(50) DEFAULT NULL,
  "usr_major" varchar(50) DEFAULT NULL,
  "usr_class" varchar(50) DEFAULT NULL,
  "usr_phone" varchar(50) DEFAULT NULL,
  "usr_email" varchar(50) DEFAULT NULL,
  "usr_register_date" timestamp NOT NULL DEFAULT CURRENT_DATE,
  "usr_image" BLOB DEFAULT NULL,
  UNIQUE ("usr_id" ASC)
);

-- ----------------------------
-- Auto increment value for apply
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 40 WHERE name = 'apply';

-- ----------------------------
-- Auto increment value for books
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 142 WHERE name = 'books';

-- ----------------------------
-- Auto increment value for id_type
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 47311 WHERE name = 'id_type';

-- ----------------------------
-- Auto increment value for managers
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 8 WHERE name = 'managers';

-- ----------------------------
-- Auto increment value for new_ids
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 424240 WHERE name = 'new_ids';

-- ----------------------------
-- Auto increment value for records
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 22 WHERE name = 'records';

-- ----------------------------
-- Auto increment value for users
-- ----------------------------
UPDATE "sqlite_sequence" SET seq = 97 WHERE name = 'users';

PRAGMA foreign_keys = true;
