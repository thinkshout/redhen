<?php
/**
 * @file
 * Hooks provided by the RedHen contact module.
 */
 
/**
 * Prevent a contact record from being deleted.
 *
 * @param string $contact 
 * @return void
 */
function hook_redhen_contact_can_delete($contact) {
  
}

/**
 * Allows you to prepare contact data before it is saved.
 *
 * @param $contact
 *   The contact object to be saved.
 * @return void
 */
function hook_redhen_contact_presave(&$contact) {

}

/**
 * undocumented function
 *
 * @param object $contact 
 *   The contact object being inserted.
 * @return void
 */
function hook_redhen_contact_insert($contact) {
  
}

/**
 * undocumented function
 *
 * @param object $contact 
 *   The contact object being updated.
 * @return void
 */
function hook_redhen_contact_update($contact) {
  
}

/**
 * undocumented function
 *
 * @param string $contact
 *   The contact object being deleted.
 * @return void
 */
function hook_redhen_contact_delete($contact) {
  
}
