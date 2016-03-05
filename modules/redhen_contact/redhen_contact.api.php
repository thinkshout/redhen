<?php

/**
 * @file
 * Describes API functions for the RedHen Contact module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alter the display name for a contact.
 *
 * @param string $name
 *   The generated name.
 * @param Drupal\redhen_contact\ContactInterface $contact
 *   The contact whose name is being generated.
 *
 * @return string
 */
function hook_redhen_contact_name_alter(&$name, Drupal\redhen_contact\ContactInterface $contact) {
  return $contact->get('last_name')->value . ', ' . $contact->get('first_name')->value;
}

/**
 * @} End of "addtogroup hooks".
 */
