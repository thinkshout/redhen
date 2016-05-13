<?php

/**
 * @file
 * Describes API functions for the RedHen Connection module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the display name for a Connection.
 *
 * @param string $name
 *   The generated name.
 * @param Drupal\redhen_connection\ConnectionInterface $Connection
 *   The Connection whose name is being generated.
 *
 * @return string
 */
function hook_redhen_connection_name_alter(&$name, Drupal\redhen_connection\ConnectionInterface $Connection) {
  // Use ALL CAPS when displaying Redhen Connection name.
  return strtoupper($Connection->get('name')->value);
}

/**
 * @} End of "addtogroup hooks".
 */
