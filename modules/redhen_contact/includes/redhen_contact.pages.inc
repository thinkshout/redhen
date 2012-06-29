<?php

/**
 * @file
 * Page callbacks for redhen contact.
 */

module_load_include('inc', 'redhen', 'includes/redhen.forms');

/**
 * Page callback for contact overview page.
 */
function redhen_contact_page() {
  // setup the header for both the query and table
  $header = array(
    'type' => array(
      'field' => 'type',
      'type' => 'property',
      'data' => 'Type',
      'specifier' => 'type'
    ),
    'first_name' => array(
      'field' => 'first_name',
      'type' => 'property',
      'data' => 'First Name',
      'specifier' => 'first_name'
    ),
    'last_name' => array(
      'field' => 'last_name',
      'type' => 'property',
      'data' => 'Last Name',
      'specifier' => 'last_name'
    ),
    'email' => array('data' => t('Email')),
    'updated' => array(
      'field' => 'updated',
      'type' => 'property',
      'data' => 'Updated',
      'sort' => 'desc',
      'specifier' => 'updated'
    ),
    'Actions',
  );

  // need to ensure the query doesn't execute when posing the form
  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = isset($_GET['bundle']) ? $_GET['bundle'] : '';
    $properties = isset($_GET['properties']) ? $_GET['properties'] : array();
    $fields = isset($_GET['fields']) ? $_GET['fields'] : array();
    $result = redhen_filter_query('redhen_contact', $header, $bundle, $properties, $fields);
  }

  $contacts = array();
  if ($result) {
    $contacts = redhen_contact_load_multiple(array_keys($result['redhen_contact']));
  }

  return array(
    'form' => drupal_get_form('redhen_filter_form', 'redhen_contact'),
    'contacts' => array(
      '#theme' => 'redhen_contact_list',
      '#contacts' => $contacts,
      '#header' => $header
    )
  );
}

/**
 * Return a list of available contacts to add or the add contact page if there's
 * only a single contact type.
 */
function redhen_contact_types_list_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the rehen_contact/add listing if only one contact type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('redhen_contact_add_list', array('content' => $content));
}

/**
 * Page callback for adding a contact.
 *
 * @param $type
 *
 * @return array|mixed
 */
function redhen_contact_add_page($type) {
  $contact = entity_get_controller('redhen_contact')->create(array('type' => $type));
  module_load_include('inc', 'redhen_contact', 'includes/redhen_contact.forms');
  return drupal_get_form('redhen_contact_contact_form', $contact);
}
