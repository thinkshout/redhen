<?php

/**
 * The class used for model type entities
 */
class RedhenDonationType extends Entity {

  public $name;
  public $label;
  public $locked;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_donation_type');
  }

}
