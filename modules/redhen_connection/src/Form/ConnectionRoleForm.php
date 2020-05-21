<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ConnectionRoleForm.
 *
 * @package Drupal\redhen_connection\Form
 */
class ConnectionRoleForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    $redhen_connection_role = $this->entity;
    $form['label'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_connection_role->label(),
      '#description' => $this->t("Label for the Connection Role."),
      '#required' => TRUE,
    ];

    $form['id'] = [
      '#type' => 'machine_name',
      '#default_value' => $redhen_connection_role->id(),
      '#machine_name' => [
        'exists' => '\Drupal\redhen_connection\Entity\ConnectionRole::load',
      ],
      '#disabled' => !$redhen_connection_role->isNew(),
    ];

    // Permissions.
    /** @var \Drupal\redhen_connection\ConnectionTypeInterface $connection_type */
    $connection_type = $this->getEntityFromRouteMatch($this->getRouteMatch(), 'redhen_connection_type');

    // @todo Change getEndpointEntityTypeId to Ids and take no argument to get all endpoints.
    $endpoints = [];
    $endpoints[] = $connection_type->getEndpointEntityTypeId(1);
    $endpoints[] = $connection_type->getEndpointEntityTypeId(2);

    // At least one endpoint is a contact so we can have permissions.
    if (in_array('redhen_contact', $endpoints)) {
      $form['permissions'] = [
        '#type' => 'fieldset',
        '#title' => $this->t('Permissions'),
      ];
      // Field operations.
      $operations = [
        'view' => $this->t('View'),
        'view label' => $this->t('View label'),
        'update' => $this->t('Update'),
        'delete' => $this->t('Delete'),
      ];
      // Existing values.
      $existing_permissions = $redhen_connection_role->get('permissions');

      // Load up plugin definitions.
      $connection_plugin_manager = \Drupal::service('plugin.manager.connection_permission');
      $plugin_definitions = $connection_plugin_manager->getDefinitions();

      // Add permissions for each plugin definition.
      foreach ($plugin_definitions as $plugin_id => $plugin_definition) {
        $plugin_instance = $connection_plugin_manager->createInstance($plugin_id);
        $permission_key = $plugin_instance->getPermissionKey();
        $form['permissions'][$permission_key] = [
          '#type' => 'checkboxes',
          '#options' => $operations,
          '#title' => $this->t(':label', [':label' => $plugin_instance->get('label')]),
          '#default_value' => (!empty($existing_permissions[$permission_key])) ? $existing_permissions[$permission_key] : [],
          '#description' => $this->t(':description', [':description' => $plugin_instance->get('description')]),
        ];
      }
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $redhen_connection_role = $this->entity;
    // Get connection type entity from the route.
    $connection_type = $this->getEntityFromRouteMatch($this->getRouteMatch(), 'redhen_connection_type');
    // Set connection type property based on the route param.
    $redhen_connection_role->set('connection_type', $connection_type->id());

    // Load up plugin definitions.
    $connection_plugin_manager = \Drupal::service('plugin.manager.connection_permission');
    $plugin_definitions = $connection_plugin_manager->getDefinitions();

    $permissions = [];

    // Build array of permissions.
    foreach ($plugin_definitions as $plugin_id => $plugin_definition) {
      $plugin_instance = $connection_plugin_manager->createInstance($plugin_id);
      $permission_key = $plugin_instance->getPermissionKey();
      $permissions[$permission_key] = is_array($form_state->getValue($permission_key)) ? array_filter(array_values($form_state->getValue($permission_key))) : NULL;
    }

    $redhen_connection_role->set('permissions', $permissions);

    $status = $redhen_connection_role->save();
    $messenger = \Drupal::messenger();
    switch ($status) {
      case SAVED_NEW:
        $messenger->addMessage($this->t('Created the %label Connection Role.', [
          '%label' => $redhen_connection_role->label(),
        ]));
        break;

      default:
        $messenger->addMessage($this->t('Saved the %label Connection Role.', [
          '%label' => $redhen_connection_role->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_connection_role->toUrl('collection'));
  }

}
