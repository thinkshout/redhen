<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\redhen_connection\ConnectionTypeInterface;

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
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_connection_role->label(),
      '#description' => $this->t("Label for the Connection Role."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $redhen_connection_role->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\redhen_connection\Entity\ConnectionRole::load',
      ),
      '#disabled' => !$redhen_connection_role->isNew(),
    );

    // Permissions.
    /** @var ConnectionTypeInterface $connection_type */
    $connection_type = $this->getEntityFromRouteMatch($this->getRouteMatch(), 'redhen_connection_type');

    // @todo Change getEndpointEntityTypeId to Ids and take no argument to get all endpoints.
    $endpoints = [];
    $endpoints[] = $connection_type->getEndpointEntityTypeId(1);
    $endpoints[] = $connection_type->getEndpointEntityTypeId(2);

    // At least one endpoint is a contact so we can have permissions.
    if (in_array('redhen_contact', $endpoints)) {
      $form['permissions'] = array(
        '#type' => 'fieldset',
        '#title' => $this->t('Permissions'),
      );

      // Standard permissions.
      $operations = ['view' => $this->t('View'), 'view label' => $this->t('View label'), 'update' => $this->t('Update'), 'delete' => $this->t('Delete')];
      $existing_permissions = $redhen_connection_role->get('permissions');
      // User's connection plus other connections.
      // @todo consider using this for connected connections and extend "own connection" to include user's own connections.
      $form['permissions']['connection'] = array(
        '#type' => 'checkboxes',
        '#options' => $operations,
        '#title' => $this->t('Connection'),
        '#default_value' => (!empty($existing_permissions['connection'])) ? $existing_permissions['connection'] : [],
        '#description' => $this->t('Applies to both the current user\'s connection and secondary connections. Sitewide permissions will override this setting.'),
      );

      // The non-contact endpoint entity type, if there is one.
      $entity_type = array_diff($endpoints, ['redhen_contact']);

      // Other endpoint permissions.
      $form['permissions']['entity'] = array(
        '#type' => 'checkboxes',
        '#options' => $operations,
        '#title' => $this->t('Entity'),
        '#default_value' => (!empty($existing_permissions['entity'])) ? $existing_permissions['entity'] : [],
        '#description' => $this->t('Sitewide permissions will override this setting.'),
        '#access' => !empty($entity_type),
      );
      // Connected Contact permissions.
      $form['permissions']['contact'] = array(
        '#type' => 'checkboxes',
        '#options' => $operations,
        '#title' => $this->t('Secondary Contact'),
        '#default_value' => (!empty($existing_permissions['contact'])) ? $existing_permissions['contact'] : [],
        '#description' => $this->t('A contact connected to the same entity via connection of the same type. Sitewide permissions will override this setting.'),
      );

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

    // Build array of permissions.
    $permissions = [
      'connection' => is_array($form_state->getValue('connection')) ? array_filter(array_values($form_state->getValue('connection'))) : NULL,
      'entity' => is_array($form_state->getValue('entity')) ? array_filter(array_values($form_state->getValue('entity'))) : NULL,
      'contact' => is_array($form_state->getValue('contact')) ? array_filter(array_values($form_state->getValue('contact'))) : NULL,
    ];

    $redhen_connection_role->set('permissions', $permissions);

    $status = $redhen_connection_role->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Connection Role.', [
          '%label' => $redhen_connection_role->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Connection Role.', [
          '%label' => $redhen_connection_role->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_connection_role->toUrl('collection'));
  }

}
