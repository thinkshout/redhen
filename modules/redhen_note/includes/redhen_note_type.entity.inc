<?php

/**
 * @file
 * Redhen Organization type classes.
 */


/**
 * The class used for model type entities
 */
class RedhenNoteType extends Entity {

  public $name;
  public $label;
  public $locked;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_note_type');
  }

}

/**
 * Redhen Contact Type UI controller.
 */
class RedhenNoteTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage note entity types, including
		  adding and removing fields and the display of fields.';
    return $items;
  }

}
