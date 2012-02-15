<?php

/**
 * @file
 * Redhen Contact type classes.
 */


/**
 * The class used for model type entities
 */
class RedhenContactType extends Entity {

  public $name;
  public $label;
  public $locked;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_contact_type');
  }

}

/**
 * Redhen Contact Type UI controller.
 */
class RedhenContactTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage contact entity types, including
		  adding and removing fields and the display of fields.';
    return $items;
  }

}
