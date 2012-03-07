<?php

/**
 * @file
 * Page callbacks for redhen membership.
 */

module_load_include('inc', 'redhen', 'includes/redhen.forms');

/**
 * Page callback for membership overview page.
 */
function redhen_membership_page($entity = NULL, $entity_type = NULL) {

  // setup the header for both the query and table
  $header = array(
    'Name',
    'type' => array('field' => 'type', 'type' => 'property', 'data' => 'Type', 'sort' => 'asc', 'specifier' => 'type'),
    'start_date' => array('field' => 'start_date', 'type' => 'property', 'data' => 'Start Date', 'sort' => 'asc', 'specifier' => 'start_state'),
    'expire_date' => array('field' => 'expire_date', 'type' => 'property', 'data' => 'Expiration Date', 'sort' => 'asc', 'specifier' => 'expire_date'),
    'label' => array('field' => 'label', 'type' => 'property', 'data' => 'Label', 'sort' => 'asc', 'specifier' => 'label'),
    'state' => array('field' => 'state', 'type' => 'property', 'data' => 'State', 'sort' => 'desc', 'specifier' => 'redhen_state'),
    'Actions',
  );

  // need to ensure the query doesn't execute when posing the form
  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = isset($_GET['bundle']) ? $_GET['bundle'] : '';
    $properties = isset($_GET['properties']) ? $_GET['properties'] : array();
    $fields = isset($_GET['fields']) ? $_GET['fields'] : array();
    // If page is for a specific entity, add that entity_type and id to the property filters
    if (isset($entity)) {
      $entity_filter = array(
        'entity_type' => $entity_type,
        'entity_id' => entity_id($entity_type, $entity),
      );
      $properties = array_merge($properties, $entity_filter);
    }
    $result = redhen_filter_query('redhen_membership', $header, $bundle, $properties, $fields);
  }

  // If page is for a redhen contact, run a second query for memberships on
  // affiliated organizations
  $org_membership_keys = array();
  if ($entity_type == 'redhen_contact') {
    $entity_ids = array();
    $relations = redhen_relation_relations($entity, 'redhen_affiliation');
    foreach ($relations as $relation => $related_entities) {
      foreach ($related_entities as $id => $related_entity) {
        $entity_ids[] = $related_entity->internalIdentifier();
      }
    }
    if ($entity_ids) {
      // Unset previous entity_type and entity_id property values
      unset($properties['entity_type']);
      unset($properties['entity_id']);
      $properties['entity_type'] = 'redhen_org';
      $properties['entity_id'] = $entity_ids;

      $result_org_memberships = redhen_filter_query('redhen_membership', $header, $bundle, $properties, $fields);
      if ($result_org_memberships) {
        $org_membership_keys = array_keys($result_org_memberships['redhen_membership']);
      }
    }
  }

  $render = array(
    'form' => drupal_get_form('redhen_filter_form', 'redhen_membership'),
  );

  $rows = array();
  if ($result || $result_org_memberships) {
    $membership_keys = array_merge(array_keys($result['redhen_membership']), $org_membership_keys);
    $memberships = redhen_membership_load_multiple($membership_keys);

    foreach($memberships as $membership) {
      $archive = $membership->redhen_state == REDHEN_STATE_ACTIVE ? t('archive') : t('restore');
      $actions = array(
        l(t('view'), 'redhen/membership/' . $membership->membership_id),
        l(t('edit'), 'redhen/membership/' . $membership->membership_id . '/edit', array('query' => drupal_get_destination())),
        l($archive, 'redhen/membership/' . $membership->membership_id . '/archive', array('query' => drupal_get_destination())),
        l(t('delete'), 'redhen/membership/' . $membership->membership_id . '/delete', array('query' => drupal_get_destination())),
      );

      $membership_type = redhen_membership_type_load($membership->type);
      $data = array(
          $membership->entity->label(),
          $membership_type->label,
          ($membership->start_date != '') ? date('m/d/Y', strtotime($membership->start_date)) : '',
          ($membership->expire_date != '') ? date('m/d/Y', strtotime($membership->expire_date)) : '',
          $membership->label,
          $membership->redhen_state == REDHEN_STATE_ACTIVE ? t('active') : t('archived'),
          implode(' | ', $actions)
      );

      $row_classes = array();
      // Set a class for the row depending on if the membership is active or archived
      $row_classes[] = $membership->redhen_state == REDHEN_STATE_ACTIVE ? 'active' : 'archived';

      $rows[] = array(
        'data' => $data,
        'class' => $row_classes,
        );
    }

    $render['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows
    );
    $render['pager'] = array(
      '#theme' => 'pager'
    );
  }
  else {
    // no results, set a message
    $render['no-result'] = array(
      '#type' => 'markup',
      '#markup' => t('Sorry, there are no memberships that match your criteria.'),
    );
  }

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

  $render = array(
    '#theme' => 'redhen_membership_add_list',
    '#content' => $content,
  );

  return $render;

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
