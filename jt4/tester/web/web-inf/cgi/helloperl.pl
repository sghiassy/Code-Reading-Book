#!/usr/bin/perl

	$t	= "Hello World!";
	print	<<EOT;
Content-type: text/html

	<Title> $t </Title>
	<H1>	$t </H1>
EOT
