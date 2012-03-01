<?php

/**
 * @file
 * Page callbacks for redhen membership.
 */

/**
 * Page callback for membership overview page.
 */
function redhen_membership_page(RedhenOrg $org = NULL) {
  $memberships = array();

  $query = new EntityFieldQuery();

  // setup the header for both the query and table
  $header = array(
    'label' => array('field' => 'label', 'type' => 'property', 'data' => 'Label', 'sort' => 'asc', 'specifier' => 'label'),
    'Actions',
  );

  // build the query
  $query
    ->entityCondition('entity_type', 'redhen_membership', '=')
    ->tablesort($header)
    ->pager(10);

  if (isset($org->org_id) && !empty($org->org_id)) {
    $query->propertyCondition('org_id', $org->org_id, '=');
  }

  // set the membership type
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
    $memberships = redhen_membership_load_multiple(array_keys($result['redhen_membership']));

    foreach($memberships as $membership) {
      $actions = array(
        l('view', 'redhen/org/' . $membership->org_id . '/memberships/' . $membership->membership_id),
        l('edit', 'redhen/org/' . $membership->org_id . '/memberships/' . $membership->membership_id . '/edit', array('query' => drupal_get_destination())),
        l('archive', 'redhen/org/' . $membership->org_id . '/memberships/' . $membership->membership_id . '/archive', array('query' => drupal_get_destination())),
        l('delete', 'redhen/org/' . $membership->org_id . '/memberships/' . $membership->membership_id . '/delete', array('query' => drupal_get_destination())),
      );
      $rows[] = array(
        'data' => array($membership->label, implode(' | ', $actions))
      );
    }
  }
  else {
    // no results, set a message
    $rows[] = array(
      'data' => array(array('data' => t('Sorry, there are no memberships that match your criteria.'), 'colspan' => 3))
    );
  }

  $entity_info = entity_get_info('redhen_membership');
  module_load_include('inc', 'redhen_membership', 'includes/redhen_membership.forms');
  $form = drupal_get_form('redhen_membership_filter_form', $entity_info);

  // return a themed table
  return drupal_render($form) . theme('table', array('header' => $header, 'rows' => $rows)) . theme('pager');
}

/**
 * Return a list of available memberships to add or the add membership page if there's
 * only a single membership type.
 */
function redhen_membership_types_list_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the rehen_membership/add listing if only one membership type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  return theme('redhen_membership_add_list', array('content' => $content));
}

/**
 * Page callback for adding a membership.
 *
 * @param $type
 * @return array|mixed
 */
function redhen_membership_add_page($org = NULL) {

  $membership = entity_get_controller('redhen_membership')->create(array('type' => $type));
  module_load_include('inc', 'redhen_membership', 'includes/redhen_membership.forms');
  return drupal_get_form('redhen_membership_membership_form', $membership, $org);
}
