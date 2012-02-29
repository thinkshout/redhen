<?php

/**
 * @file
 * Page callbacks for redhen organizations.
 */

/**
 * Page callback for org overview page.
 */
function redhen_org_page() {
  $orgs = array();

  $query = new EntityFieldQuery();

  // setup the header for both the query and table
  $header = array(
    'label' => array('field' => 'label', 'type' => 'property', 'data' => 'Label', 'sort' => 'asc', 'specifier' => 'label'),
    'Actions',
  );

  // build the query
  $query
    ->entityCondition('entity_type', 'redhen_org', '=')
    ->tablesort($header)
    ->pager(10);

  // set the org type
  if (isset($_GET['type']) && !empty($_GET['type'])) {
    $query->propertyCondition('type', $_GET['type'], '=');
  }

  // add prop filters
  if (isset($_GET['properties'])) {
    foreach($_GET['properties'] as $key => $value) {
      if (!empty($value)) {
        $query->propertyCondition($key, $value, '=');
      }
    }
  }

  // add field filters
  if (isset($_GET['fields'])) {
    foreach($_GET['fields'] as $key => $value) {
      if (!empty($value)) {
        $query->fieldCondition($key, NULL, $value, '=');
      }
    }
  }

  $result = $query->execute();

  $rows = array();
  if ($result) {
    $orgs = redhen_org_load_multiple(array_keys($result['redhen_org']));

    foreach($orgs as $org) {
      $actions = array(
        l('view', 'redhen/org/' . $org->org_id),
        l('edit', 'redhen/org/' . $org->org_id . '/edit', array('query' => drupal_get_destination())),
        l('archive', 'redhen/org/' . $org->org_id . '/archive', array('query' => drupal_get_destination())),
        l('delete', 'redhen/org/' . $org->org_id . '/delete', array('query' => drupal_get_destination())),
      );
      $rows[] = array(
        'data' => array($org->label, implode(' | ', $actions))
      );
    }
  }
  else {
    // no results, set a message
    $rows[] = array(
      'data' => array(array('data' => t('Sorry, there are no organizations that match your criteria.'), 'colspan' => 3))
    );
  }

  $entity_info = entity_get_info('redhen_org');
  $property_info = entity_get_property_info('redhen_org');
  module_load_include('inc', 'redhen_org', 'includes/redhen_org.forms');
  $form = drupal_get_form('redhen_org_filter_form', $entity_info, $property_info);

  // return a themed table
  return drupal_render($form) . theme('table', array('header' => $header, 'rows' => $rows)) . theme('pager');
}

/**
 * Page callback for adding an organization.
 *
 * @param string $type
 *   The type of organization to return a form for.
 */
function redhen_org_types_list_page($type = 'default') {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the redhen_org/add listing if only one contact type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('redhen_org_add_list', array('content' => $content));
}

/**
 * Page callback for adding an organization.
 *
 * @param $type
 * @return array|mixed
 */
function redhen_org_add_page($type) {
  $org = entity_get_controller('redhen_org')->create(array('type' => $type));
  module_load_include('inc', 'redhen_org', 'includes/redhen_org.forms');
  return drupal_get_form('redhen_org_org_form', $org);
}
