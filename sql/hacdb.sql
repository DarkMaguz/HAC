CREATE TABLE  `hac01`.`collision` (
  `id` int(10) unsigned NOT NULL,
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `wu_id` int(10) unsigned NOT NULL,
  `x_pos` bigint(20) unsigned NOT NULL,
  `y_pos` bigint(20) unsigned NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`completed` (
  `id` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `x_size` bigint(20) unsigned NOT NULL,
  `y_password` tinyblob NOT NULL,
  `y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',
  `wu_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`,`wu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`moved` (
  `id_from` int(10) unsigned NOT NULL,
  `id_to` int(10) unsigned NOT NULL,
  `wu_from` int(10) unsigned NOT NULL,
  `wu_to` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id_from`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`user` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` tinytext NOT NULL,
  `email` tinytext NOT NULL,
  `score` bigint(20) unsigned default '0',
  `validation` char(12) NOT NULL,
  `wu_completed` int(10) unsigned default '0',
  `wu_inprogress` int(10) unsigned default '0',
  `wu_total` int(10) unsigned default '0',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`workunit` (
  `id` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `x_size` bigint(20) unsigned NOT NULL,
  `x_completed` bigint(20) unsigned default '0',
  `y_password` tinyblob NOT NULL,
  `y_size` bigint(20) unsigned NOT NULL COMMENT 'memsize = y_size*256',
  `time` bigint(20) unsigned NOT NULL,
  `wu_id` int(10) unsigned NOT NULL default '0',
  `benchmark` bigint(20) unsigned NOT NULL default '0',
  PRIMARY KEY  (`id`,`wu_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE  `hac01`.`wubank` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `y_password` tinyblob NOT NULL,
  `y_size` int(10) unsigned NOT NULL,
  `x_password` tinyblob NOT NULL,
  `open` tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (`id`,`y_size`)
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;

