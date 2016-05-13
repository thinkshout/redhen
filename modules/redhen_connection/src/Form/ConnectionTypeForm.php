<?php

/**
 * @file
 * Contains \Drupal\redhen_connection\Form\ConnectionTypeForm.
 */

namespace Drupal\redhen_connection\Form;

use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class ConnectionTypeForm.
 *
 * @package Drupal\redhen_connection\Form
 */
class ConnectionTypeForm extends EntityForm {
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    // Prepare the list of relatable entity types.
    $entity_types = $this->entityTypeManager->getDefinitions();
    $endpoint_entity_types = array_map(function ($entity_type) {
      return $entity_type->getLabel();
    }, $entity_types);

    $form['#tree'] = TRUE;
    /** @var \Drupal\redhen_connection\Entity\ConnectionType $redhen_connection_type */
    $redhen_connection_type = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $redhen_connection_type->label(),
      '#description' => $this->t("Label for the Connection type."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $redhen_connection_type->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\redhen_connection\Entity\ConnectionType::load',
      ),
      '#disabled' => !$redhen_connection_type->isNew(),
    );

    $form['entityType1'] = [
      '#type' => 'select',
      '#title' => $this->t('Endpoint 1 entity type'),
      '#default_value' => $redhen_connection_type->getEndpointEntityTypeId('1'),
      '#options' => $endpoint_entity_types,
      '#empty_value' => '',
      '#disabled' => !$redhen_connection_type->isNew(),
    ];

    $form['entityType2'] = [
      '#type' => 'select',
      '#title' => $this->t('Endpoint 2 entity type'),
      '#default_value' => $redhen_connection_type->getEndpointEntityTypeId('2'),
      '#options' => $endpoint_entity_types,
      '#empty_value' => '',
      '#disabled' => !$redhen_connection_type->isNew(),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $redhen_connection_type = $this->entity;
    $status = $redhen_connection_type->save();

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Connection type.', [
          '%label' => $redhen_connection_type->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Connection type.', [
          '%label' => $redhen_connection_type->label(),
        ]));
    }
    $form_state->setRedirectUrl($redhen_connection_type->urlInfo('collection'));
  }

}
