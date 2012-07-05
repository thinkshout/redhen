<?php

/**
 * @file
 * RedHen Membership Views integration
 */

/**
 * Implement hook_views_data_alter().
 *
 * Create relationships between redhen_memberships, and their related entities.
 * Provide additional fields for entities with redhen_memberships.
 */
function redhen_membership_views_data_alter(&$data) {

  $entity_info = entity_get_info();

  foreach ($entity_info as $entity_type => $info) {

    // entity to membership.
    $data[$info['base table']]['redhen_membership_rel'] = array(
      'group' => t('Membership'),
      'title' => t('@entity to Membership',
        array('@entity' => drupal_ucfirst($info['label']))),
      'help' => t('The Membership associated with the @entity entity.',
        array('@entity' => drupal_ucfirst($info['label']))),
      'relationship' => array(
        'entity' => $entity_type,
        'handler' => 'redhen_membership_handler_relationship',
        'label' => t('@entity being the Membership',
          array('@entity' => $entity_type)),
        'base' => 'redhen_membership',
        'base field' => 'entity_id',
        'relationship field' => $info['entity keys']['id'],
      ),
    );

    // entity to membership.
    $data[$info['base table']]['redhen_membership_active_member_type'] = array(
      'group' => t('Membership'),
      'title' => t('@entity active membership type(s)',
        array('@entity' => drupal_ucfirst($info['label']))),
      'help' => t('The active membership type(s) of the @entity entity.',
        array('@entity' => drupal_ucfirst($info['label']))),
      'field' => array(
        'handler' => 'redhen_membership_handler_field_active_member_type',
      ),
    );

    // Membership to entity.
    $data['redhen_membership']['redhen_membership_related_' . $entity_type] = array(
      'group' => t('Membership'),
      'title' => t('Membership to @entity',
        array('@entity' => drupal_ucfirst($info['label']))),
      'help' => t('The @entity entity that is associated with the Membership.',
        array('@entity' => $info['label'])),
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'label' => t('@entity from Membership',
          array('@entity' => $entity_type)),
        'base' => $info['base table'],
        'base field' => $info['entity keys']['id'],
        'relationship field' => 'entity_id',
      ),
    );

  }
}
