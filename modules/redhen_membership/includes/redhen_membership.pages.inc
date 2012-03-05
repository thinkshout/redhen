<?php

/**
 * @file
 * Page callbacks for redhen membership.
 */

module_load_include('inc', 'redhen', 'includes/redhen.forms');

/**
 * Page callback for membership overview page.
 */
function redhen_membership_page(RedhenOrg $org = NULL) {

  // setup the header for both the query and table
  $header = array(
    'label' => array('field' => 'label', 'type' => 'property', 'data' => 'Label', 'sort' => 'asc', 'specifier' => 'label'),
    'Actions',
  );

  // need to ensure the query doesn't execute when posing the form
  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = isset($_GET['bundle']) ? $_GET['bundle'] : '';
    $properties = isset($_GET['properties']) ? $_GET['properties'] : array();
    $fields = isset($_GET['fields']) ? $_GET['fields'] : array();
    $result = redhen_filter_query('redhen_membership', $header, $bundle, $properties, $fields);
  }

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

  $render = array(
    'form' => drupal_get_form('redhen_filter_form', 'redhen_membership'),
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows
    ),
    'pager' => array(
      '#theme' => 'pager'
    )
  );

  return $render;
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
