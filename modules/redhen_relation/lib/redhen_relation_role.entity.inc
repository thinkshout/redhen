<?php
/**
 * @file
 * Redhen Relation Role entity classes
 */

/**
 * The class used for relation role entities.
 */
class RedhenRelationRole extends Entity {

  // @codingStandardsIgnoreStart
  public
    $label = '';
  // @codingStandardsIgnoreEnd

  /**
   * Override parent constructor.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_relation_role');
  }


}
