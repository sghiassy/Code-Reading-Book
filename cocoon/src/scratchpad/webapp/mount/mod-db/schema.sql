
-- this is the hsqldb schema file
-- to adapt it to another RDBMS, replace column type identity
-- with appropriate autoincrement type, e.g. SERIAL for informix
-- you might want to add "on delete cascade" to foreign keys in
-- taböe user_groups

create table user (
	uid integer identity primary key,
	name varchar(50),
	firstname varchar(50),
	uname varchar(20),
	unique (uname)
);
create table groups (
	gid integer identity primary key,
	gname varchar(20),
	unique (gname)
);

create table user_groups (
	uid integer,
	gid integer,
	primary key (uid,gid),
	foreign key (uid) references user(uid),
	foreign key (gid) references groups(gid)
);