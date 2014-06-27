<?php
/**
 * @file
 * Redhen Relation Role entity classes
 */

/**
 * The class used for relation role entities.
 */
class RedHenRelationRole extends Entity {

  // @codingStandardsIgnoreStart
  public
    $label = '',
    $default_role = 0;
  // @codingStandardsIgnoreEnd

  /**
   * Override parent constructor.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_relation_role');
  }


}