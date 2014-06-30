# MySQL dump 6.4
#
# Host: localhost    Database: test
#--------------------------------------------------------
# Server version	3.22.27

#
# Table structure for table 'department'
#
CREATE TABLE department (
  id int(11) DEFAULT '0' NOT NULL,
  name varchar(255) DEFAULT '' NOT NULL,
  PRIMARY KEY (id)
);

#
# Dumping data for table 'department'
#

INSERT INTO department VALUES (1,'Programmers');
INSERT INTO department VALUES (2,'Loungers');

#
# Table structure for table 'employee'
#
CREATE TABLE employee (
  id int(11) DEFAULT '0' NOT NULL,
  department_id int(11) DEFAULT '0' NOT NULL,
  name varchar(255) DEFAULT '' NOT NULL,
  PRIMARY KEY (id)
);

#
# Dumping data for table 'employee'
#

INSERT INTO employee VALUES (1,1,'Donald Ball');
INSERT INTO employee VALUES (2,1,'Stefano Mazzocchi');
INSERT INTO employee VALUES (3,2,'Pierpaolo Fumagalli');
