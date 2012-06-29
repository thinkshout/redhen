<?php
/**
 * @file
 * Hooks provided by the RedHen contact module.
 */

/**
 * All RedHen custom entities, including contacts, organizations, memberships,
 * notes, and engagements are based on Entity API Entity controller class and
 * inherit all of it's CRUD hooks. It also provides the following additional
 * hooks.
 */

/**
 * Prevent a contact record from being deleted.
 *
 * @param string $contact
 *
 * @return void
 */
function hook_redhen_contact_can_delete($contact) {

}

/**
 * Prevent an organization record from being deleted.
 *
 * @param RedhenOrg $org
 *
 * @return void
 */
function hook_redhen_org_can_delete(RedhenOrg $org) {

}

/**
 * Alter the display name for a contact.
 *
 * @param string $name
 * @param RedhenContact $contact
 *
 * @return string
 */
function hook_redhen_contact_name_alter(&$name, RedhenContact $contact) {
  return $contact->last_name . ', ' . $contact->last_name;
}

/**
 * Allow modules to act before an entity is deleted.
 *
 * @param string $entity_type ( redhen_org | redhen_contact )
 * @param Entity $entity
 */
function hook_redhen_entity_predelete($entity_type, $entity) {

}

/**
 * Provide a form API element exposed as a RedHen setting.
 *
 * @return array
 */
function hook_redhen_settings() {
  return array(
    'redhen_contact_connect_users' => array(
      '#type' => 'checkbox',
      '#title' => t('Connect users to Redhen contacts'),
      '#description' => t('If checked, Redhen will attempt to connect Drupal users to Redhen contacts by matching email addresses when a contact is updated.'),
      '#default_value' => variable_get('redhen_contact_connect_users', FALSE)
    )
  );
}
