<?php
/**
 * @file
 * Entity Type file.
 */

/**
 * The class used for model type entities
 */
class RedhenDonationType extends Entity {

  public $name;
  public $label;
  public $locked;

  /**
   * Type constructor.
   *
   * @param array $values
   *   Values array.
   */
  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_donation_type');
  }

}
