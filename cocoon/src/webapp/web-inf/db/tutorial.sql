#
# Tutorial DB
#

#
# Table structure for table 'department'
#
CREATE TABLE department (
  id number(12) DEFAULT '0' NOT NULL,
  name varchar(64) DEFAULT '' NOT NULL,
  PRIMARY KEY (id)
);

#
# Data for table 'department'
#

INSERT INTO department VALUES (1,'Development');
INSERT INTO department VALUES (2,'Management');
INSERT INTO department VALUES (3,'Testors');

#
# Table structure for table 'employee'
#
CREATE TABLE employee (
  id number(12) DEFAULT '0' NOT NULL,
  department_id number(12) DEFAULT '0' NOT NULL,
  name varchar(64) DEFAULT '' NOT NULL,
  PRIMARY KEY (id)
);

#
# Foreign Keys
#
ALTER TABLE employee ADD (
  CONSTRAINT fkdepartment FOREIGN KEY(DEPARTMENT_ID)
  REFERENCES DEPARTMENT(ID)
  ON DELETE CASCADE
);

#
# Data for table 'employee'
#

INSERT INTO employee VALUES (1,1,'Donald Ball');
INSERT INTO employee VALUES (2,1,'Stefano Mazzocchi');
INSERT INTO employee VALUES (3,2,'Pierpaolo Fumagalli');
INSERT INTO employee VALUES (4,4,'Torsten Curdt');
