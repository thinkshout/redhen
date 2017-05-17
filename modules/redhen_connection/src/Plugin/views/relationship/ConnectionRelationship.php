<?php

namespace Drupal\redhen_connection\Plugin\views\relationship;

use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Plugin\views\relationship\RelationshipPluginBase;

/**
 * Views relationship plugin for datasources.
 *
 * @ingroup views_relationship_handlers
 *
 * @ViewsRelationship("redhen_connection")
 */
class ConnectionRelationship extends RelationshipPluginBase {

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface|null
   */
  protected $entityTypeManager;

  /**
   * Retrieves the entity type manager.
   *
   * @return \Drupal\Core\Entity\EntityTypeManagerInterface
   *   The entity type manager.
   */
  public function getEntityTypeManager() {
    return $this->entityTypeManager ?: \Drupal::entityTypeManager();
  }

  /**
   * Sets the entity type manager.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The new entity type manager.
   *
   * @return $this
   */
  public function setEntityTypeManager(EntityTypeManagerInterface $entity_type_manager) {
    $this->entityTypeManager = $entity_type_manager;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  protected function defineOptions() {
    $options = parent::defineOptions();
    $options['status']['default'] = TRUE;
    $options['status__endpoint_1']['default'] = TRUE;
    $options['status__endpoint_2']['default'] = TRUE;
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);

    $form['required']['#access'] = FALSE;

    $form['status'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Connection Status'),
      '#description' => $this->t('Filter based on status of the Connection Entity.'),
      '#options' => ['_any' => $this->t('Any'), '0' => $this->t('Inactive'), '1' => $this->t('Active')],
      '#default_value' => $this->options['status'],
      '#weight' => -1,
    ];

    $form['status__endpoint_1'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Connection Endpoint 1 Status'),
      '#description' => $this->t('Filter based on status of the Connection Endpoint 1 Entity.'),
      '#options' => ['_any' => $this->t('Any'), '0' => $this->t('Inactive'), '1' => $this->t('Active')],
      '#default_value' => $this->options['status__endpoint_1'],
      '#weight' => -1,
    ];

    $form['status__endpoint_2'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Connection Endpoint 2 Status'),
      '#description' => $this->t('Filter based on status of the Connection Endpoint 2 Entity.'),
      '#options' => ['_any' => $this->t('Any'), '0' => $this->t('Inactive'), '1' => $this->t('Active')],
      '#default_value' => $this->options['status__endpoint_2'],
      '#weight' => -1,
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function query() {
    $this->alias = $this->field;
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
    $dependencies = [];

    if (!empty($this->definition['entity type'])) {
      $entity_type = $this->getEntityTypeManager()
        ->getDefinition($this->definition['entity type']);
      if ($entity_type) {
        $dependencies['module'][] = $entity_type->getProvider();
      }
    }

    return $dependencies;
  }

}
