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

  public function save() {
    $ret = parent::save();
    if ($ret) {
      $email_field = array(
        'field_name' => REDHEN_CONTACT_EMAIL_FIELD,
        'entity_type' => 'redhen_contact',
        'bundle' => $this->name,
        'label' => t('Email'),

      );
      field_create_instance($email_field);
    }
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
