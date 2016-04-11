<?php

/**
 * @file
 * RedHen Relation Views integration
 */

/**
 * Implement hook_views_data_alter().
 *
 */
function redhen_relation_views_data_alter(&$data) {

  $data['relation']['redhen_relation_primary_contact'] = array(
    'group' => t('Relation'),
    'title' => t('Is primary contact'),
    'help' => t('Boolean value of if a contact is the primary contact.'),
    'field' => array(
      'handler' => 'redhen_relation_handler_field_primary_contact',
      'click sortable' => TRUE, // This is use by the table display plugin.
    ),
    'sort' => array(
      'handler' => 'redhen_relation_handler_sort_primary_contact',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
  );

  // Use our own Views field handler for the relation edit field.
  $data['relation']['redhen_combined_edit'] = array(
    'title' => t('Combined edit link'),
    'help' => t('Provide a simple link to edit the relation entity AND the RedHen organization or contact associated with it.'),
    'field' => array(
      'handler' => 'redhen_relation_handler_field_relation_link_combined_edit',
      'click sortable' => TRUE,
      'real field' => 'rid',
      'additional fields' => array(
        'rid',
      ),
    ),
  );

  // Add the affiliation status form field to relations
  $data['relation']['redhen_relation_status_edit'] = array(
    'field' => array(
      'title' => t('RedHen Affiliation Status form field'),
      'help' => t('Edit the RedHen affiliation status form field'),
      'handler' => 'redhen_relation_handler_field_redhen_relation_status_edit',
    ),
  );
}
