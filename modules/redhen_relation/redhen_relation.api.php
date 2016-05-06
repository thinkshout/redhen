<?php

/**
 * @file
 * Describes API functions for the RedHen Relation module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the display name for a Relation.
 *
 * @param string $name
 *   The generated name.
 * @param Drupal\redhen_relation\RelationInterface $Relation
 *   The Relation whose name is being generated.
 *
 * @return string
 */
function hook_redhen_relation_name_alter(&$name, Drupal\redhen_relation\RelationInterface $Relation) {
  // Use ALL CAPS when displaying Redhen Relation name.
  return strtoupper($Relation->get('name')->value);
}

/**
 * @} End of "addtogroup hooks".
 */
