<?php

/**
 * @file
 * Redhen Membership type classes.
 */


/**
 * The class used for model type entities
 */
class RedhenMembershipType extends Entity {

  public $name;
  public $label;
  public $role_id = NULL;
  public $locked;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_membership_type');
  }

}

/**
 * Redhen Contact Type UI controller.
 */
class RedhenMembershipTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage membership entity types, including
		  adding and removing fields and the display of fields.';
    return $items;
  }

}
