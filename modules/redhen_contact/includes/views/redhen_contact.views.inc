<?php

/**
 * @file
 * Views integration for redhen_contact module.
 */

/**
 * Implements hook_views_data().
 */
function redhen_contact_views_data() {
  $data['redhen_contact']['table']['group'] = t('Contacts');

  $data['redhen_contact']['table']['base'] = array(
    'field' => 'contact_id',
    'title' => t('Contacts'),
    'help' => t('Redhen Contacts.'),
  );

  $data['redhen_contact']['contact_id'] = array(
    'title' => t('Contact ID'),
    'help' => t('The ID of the contact.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'title',
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['redhen_contact']['redhen_state'] = array(
    'title' => t('Contact State'),
    'help' => t("The state of the contact record."),
    'field' => array(
      'handler' => 'redhen_contact_handler_field_contact_state',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'title',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['redhen_contact']['revision_id'] = array(
    'title' => t('Revision ID'),
    'help' => t('The revision ID of the contact.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'title',
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['redhen_contact']['first_name'] = array(
    'title' => t('First name'),
    'help' => t("The contact's first name."),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'name field' => 'title',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['redhen_contact']['last_name'] = array(
    'title' => t('Last name'),
    'help' => t("The contact's last name."),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'name field' => 'title',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['redhen_contact']['view_contact'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a simple link to the node.'),
      'handler' => 'redhen_contact_handler_field_contact_link',
    ),
  );

  $data['redhen_contact']['edit_contact'] = array(
    'field' => array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the contact.'),
      'handler' => 'redhen_contact_handler_field_contact_link_edit',
    ),
  );

  $data['redhen_contact']['delete_contact'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the contact.'),
      'handler' => 'redhen_contact_handler_field_contact_link_delete',
    ),
  );

  return $data;
}
