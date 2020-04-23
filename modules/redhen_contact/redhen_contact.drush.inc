<?php

/**
 * @file
 * Drush tools for the Redhen Contacts module.
 */

use Drupal\redhen_contact\Entity\Contact;

/**
 * Implements hook_drush_command().
 */
function redhen_contact_drush_command() {
  $items = [];

  // Deletes the temporary node table column created by save-moderation-states.
  $items['redhen-contact-link-users'] = [
    'description' => "Link Users without Contacts to existing Contacts based on email address.

Arguments:
 check                                 The number of Users to check. (Default is 50).",
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'core' => ['8+'],
  ];

  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 *
 * Search for users without Redhen Contacts and attempt to connect them to
 * existing Contacts based on email address.
 */
function drush_redhen_contact_link_users($check = 50) {
  $query = \Drupal::database()->select('users_field_data', 'u');
  $query->leftJoin('redhen_contact', 'rc', 'rc.uid = u.uid');
  $query->fields('u', ['uid', 'mail']);
  $query->isNull('rc.uid');
  $query->range(0, $check);
  $results = $query->execute()->fetchAllAssoc('uid');
  $log = [];
  foreach ($results as $orphan) {
    $contacts = Contact::loadByMail($orphan->mail);
    if ($contacts) {
      foreach ($contacts as $contact) {
        if (!$contact->getUserId()) {
          // We have a match!
          $contact->setUserId($orphan->uid);
          $contact->save();
          $log[$orphan->uid] = $contact->id();
          continue 2;
        }
      }
    }
  }
  if (empty($log) && count($results) == $check) {
    drush_print("No User/Contact connections were created from " . count($results) . " Users found without Contacts. You may want to re-run this function with a higher limit.");
  }
  else {
    drush_print(count($log) . " User/Contact connections created from " . count($results) . " Users found without Contacts.");
  }
}
