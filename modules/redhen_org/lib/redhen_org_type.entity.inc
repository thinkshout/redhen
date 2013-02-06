<?php

/**
 * @file
 * Redhen Organization type classes.
 */


/**
 * The class used for model type entities
 */
class RedhenOrgType extends Entity {

  public $name;
  public $label;
  public $locked;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_org_type');
  }

}

