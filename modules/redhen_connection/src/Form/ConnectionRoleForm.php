<?php

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\redhen_connection\ConnectionTypeInterface;
use Drupal\redhen_connection\Entity\ConnectionType;

/**
 * Class ConnectionRoleForm.
 *
 * @package Drupal\redhen_connection\Form
 */
class ConnectionRoleForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
//  protected function prepareEntity() {
//    parent::prepareEntity();
//
//    $this->entity->connection_type = '';
//  }

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
    $connection_type_id = $this->getEntityFromRouteMatch($this->getRouteMatch(), 'redhen_connection_type');
    $connection_type = ConnectionType::load($connection_type_id);

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
      $operations = ['view', 'view label', 'update', 'delete', 'create'];
      // Connection permissions.
      $form['permissions']['connection'] = array(
        '#type' => 'checkboxes',
        '#options' => $operations,
        '#title' => $this->t('Connection'),
        '#description' => $this->t('Applies to both the current user\'s connection and secondary connections. Sitewide permissions will override this setting.'),
      );
      // Other endpoint permissions.
      // @todo hard coded, but shouldn't be.
      $form['permissions']['redhen_org'] = array(
        '#type' => 'checkboxes',
        '#options' => $operations,
        '#title' => $this->t('Entity'),
        '#description' => $this->t('Sitewide permissions will override this setting.'),
      );
      // Connected Contact permissions.
      $form['permissions']['redhen_contact'] = array(
        '#type' => 'checkboxes',
        '#options' => $operations,
        '#title' => $this->t('Secondary Contact'),
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
    $form_state->setRedirectUrl($redhen_connection_role->urlInfo('collection'));
  }

}
